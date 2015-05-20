/*
 * Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "YamlParser.h"
#include "utility/streams/FileInputStream.h"
#include "utility/FileSystem.h"
#include <yaml.h>
#include <unordered_map>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <sstream>

namespace
{
    using namespace Z;

    class YamlParser
    {
    public:
        explicit YamlParser(YamlErrorPtr& error);
        ~YamlParser();

        bool openFile(const std::string& path);
        bool openStream(InputStream* stream);

        bool parse();

        const YamlNode& rootNode() const { return m_RootNode; }

    private:
        typedef void (YamlParser::* CurrentState)(void);

        FILE* m_File;
        std::string m_FileName;
        YamlErrorPtr& m_Error;
        CurrentState m_NextState;
        yaml_event_t m_Event;
        yaml_parser_t m_Parser;
        bool m_EventValid;
        bool m_ParserValid;
        std::unordered_map<std::string, YamlNode> m_Anchors;
        YamlNode m_RootNode;

        bool fetchNextParserEvent();
        bool setParseError(const std::string& message);

        bool parseValue(YamlNode& node);
        bool parseAlias(YamlNode& node);
        bool parseScalarValue(YamlNode& node);
        bool parseSequenceValue(YamlNode& node);
        bool parseMappingValue(YamlNode& node);

        bool checkAnchor(const void* name, const YamlNode& node);
    };

    YamlParser::YamlParser(YamlErrorPtr& error)
        : m_File(nullptr)
        , m_Error(error)
        , m_NextState(nullptr)
        , m_EventValid(false)
        , m_ParserValid(false)
    {
    }

    YamlParser::~YamlParser()
    {
        if (m_EventValid)
            yaml_event_delete(&m_Event);

        if (m_ParserValid)
            yaml_parser_delete(&m_Parser);

        if (m_File)
            fclose(m_File);
    }

    bool YamlParser::openFile(const std::string& path)
    {
        assert(m_File == nullptr);
        assert(!m_ParserValid);

        m_File = fopen(path.c_str(), "r");
        if (!m_File)
        {
            int err = errno;
            std::stringstream ss;
            ss << "Unable to open file \"" << path << "\": " << strerror(err);
            m_Error.reset(new std::string(ss.str()));
            return false;
        }

        if (!yaml_parser_initialize(&m_Parser))
        {
            m_Error.reset(new std::string("Unable to initialize YAML parser."));
            return false;
        }

        m_ParserValid = true;
        m_FileName = path;
        yaml_parser_set_input_file(&m_Parser, m_File);

        return true;
    }

    bool YamlParser::openStream(InputStream* stream)
    {
        assert(m_File == nullptr);
        assert(!m_ParserValid);

        if (!yaml_parser_initialize(&m_Parser))
        {
            m_Error.reset(new std::string("Unable to initialize YAML parser."));
            return false;
        }

        m_ParserValid = true;
        m_FileName = stream->name();

        yaml_parser_set_input(&m_Parser, [](void* ud, unsigned char* buffer, size_t size, size_t * bytesRead) -> int {
            InputStream* input = reinterpret_cast<InputStream*>(ud);
            return input->readAtMost(buffer, size, bytesRead);
        }, stream);

        return true;
    }

    bool YamlParser::parse()
    {
        assert(!m_Error);
        assert(m_ParserValid);

        // Read YAML_STREAM_START_EVENT
        if (!fetchNextParserEvent())
            return false;
        if (m_Event.type != YAML_STREAM_START_EVENT)
            return setParseError("expected YAML_STREAM_START_EVENT.");

        // Read YAML_DOCUMENT_START_EVENT
        if (!fetchNextParserEvent())
            return false;
        if (m_Event.type == YAML_DOCUMENT_START_EVENT)
        {
            // Read document
            if (!fetchNextParserEvent())
                return false;
            if (!parseValue(m_RootNode))
                return false;

            // Read YAML_DOCUMENT_END_EVENT
            if (!fetchNextParserEvent())
                return false;
            if (m_Event.type != YAML_DOCUMENT_END_EVENT)
                return setParseError("expected YAML_DOCUMENT_END_EVENT.");

            // Read YAML_STREAM_END_EVENT
            if (!fetchNextParserEvent())
                return false;
        }

        // Check for YAML_STREAM_END_EVENT
        if (m_Event.type != YAML_STREAM_END_EVENT)
            return setParseError("expected YAML_STREAM_END_EVENT.");

        return true;
    }

    bool YamlParser::fetchNextParserEvent()
    {
        assert(m_ParserValid);

        if (m_EventValid)
        {
            yaml_event_delete(&m_Event);
            m_EventValid = false;
        }

        if (yaml_parser_parse(&m_Parser, &m_Event))
            m_EventValid = true;
        else
        {
            m_Event.start_mark = m_Parser.problem_mark;
            setParseError(m_Parser.problem);
        }

        return m_EventValid;
    }

    bool YamlParser::setParseError(const std::string& message)
    {
        size_t line = m_Event.start_mark.line + 1;
        size_t column = m_Event.start_mark.column + 1;

        std::stringstream ss;
        ss << m_FileName << "(" << line << ", " << column << "): " << message;
        m_Error.reset(new std::string(ss.str()));

        m_NextState = nullptr;
        return false;
    }

    bool YamlParser::parseValue(YamlNode& node)
    {
        switch (m_Event.type)
        {
        case YAML_ALIAS_EVENT:
            return parseAlias(node);

        case YAML_SCALAR_EVENT:
            return parseScalarValue(node);

        case YAML_SEQUENCE_START_EVENT:
            return parseSequenceValue(node);

        case YAML_MAPPING_START_EVENT:
            return parseMappingValue(node);

        case YAML_NO_EVENT:
        case YAML_STREAM_START_EVENT:
        case YAML_STREAM_END_EVENT:
        case YAML_DOCUMENT_START_EVENT:
        case YAML_DOCUMENT_END_EVENT:
        case YAML_SEQUENCE_END_EVENT:
        case YAML_MAPPING_END_EVENT:
            return false;
        }

        assert(false);
        return false;
    }

    bool YamlParser::parseAlias(YamlNode& node)
    {
        std::string anchor = reinterpret_cast<const char*>(m_Event.data.alias.anchor);
        auto it = m_Anchors.find(anchor);
        if (it == m_Anchors.end())
            return setParseError("use of undeclared anchor \"" + anchor + "\".");
        node = it->second;
        return true;
    }

    bool YamlParser::parseScalarValue(YamlNode& node)
    {
        std::string text(reinterpret_cast<const char*>(m_Event.data.scalar.value), m_Event.data.scalar.length);
        int line = int(m_Event.start_mark.line + 1);
        int column = int(m_Event.start_mark.column + 1);

        bool valid = false;
        switch (m_Event.data.scalar.style)
        {
        case YAML_ANY_SCALAR_STYLE: break;
        case YAML_PLAIN_SCALAR_STYLE: valid = true; node = YamlNode::plain(text, line, column); break;
        case YAML_SINGLE_QUOTED_SCALAR_STYLE: valid = true; node = YamlNode::singleQuoted(text, line, column); break;
        case YAML_DOUBLE_QUOTED_SCALAR_STYLE: valid = true; node = YamlNode::doubleQuoted(text, line, column); break;
        case YAML_LITERAL_SCALAR_STYLE: valid = true; node = YamlNode::literal(text, line, column); break;
        case YAML_FOLDED_SCALAR_STYLE: valid = true; node = YamlNode::folded(text, line, column); break;
        }

        if (!valid)
            return setParseError("invalid scalar value.");

        return checkAnchor(m_Event.data.scalar.anchor, node);
    }

    bool YamlParser::parseSequenceValue(YamlNode& node)
    {
        int line = int(m_Event.start_mark.line + 1);
        int column = int(m_Event.start_mark.column + 1);

        YamlNode::SequenceValue* items = nullptr;
        node = YamlNode::sequence(items, line, column);

        checkAnchor(m_Event.data.sequence_start.anchor, node);

        for (;;)
        {
            YamlNode value;
            if (!fetchNextParserEvent())
                return false;
            if (m_Event.type == YAML_SEQUENCE_END_EVENT)
                break;
            if (!parseValue(value))
                return false;

            items->emplace_back(value);
        }

        return true;
    }

    bool YamlParser::parseMappingValue(YamlNode& node)
    {
        int line = int(m_Event.start_mark.line + 1);
        int column = int(m_Event.start_mark.column + 1);

        YamlNode::MappingValue* items = nullptr;
        node = YamlNode::mapping(items, line, column);

        checkAnchor(m_Event.data.mapping_start.anchor, node);

        for (;;)
        {
            YamlNode key;
            if (!fetchNextParserEvent())
                return false;
            if (m_Event.type == YAML_MAPPING_END_EVENT)
                break;
            if (!parseValue(key))
                return false;
            if (!key.isText())
                return setParseError("only text elements are allowed as keys.");

            YamlNode value;
            if (!fetchNextParserEvent())
                return false;
            if (!parseValue(value))
                return false;

            items->insert({ key.toString(), value });
        }

        return true;
    }

    bool YamlParser::checkAnchor(const void* name, const YamlNode& node)
    {
        if (name)
        {
            std::string anchorName = reinterpret_cast<const char*>(name);
            auto it = m_Anchors.insert({ anchorName, node });
            if (!it.second)
                return setParseError("duplicate anchor \"" + anchorName + "\".");
        }
        return true;
    }
}

namespace Z
{
    /*
    YamlNode yamlParseFile(const std::string& file, YamlErrorPtr& error)
    {
        YamlParser parser(error);

        if (!parser.openFile(file))
            return YamlNode();

        if (!parser.parse())
            return YamlNode();

        return parser.rootNode();
    }
    */

    YamlNode yamlParseStream(InputStream* stream, YamlErrorPtr& error)
    {
        YamlParser parser(error);

        if (!stream) {
            error.reset(new std::string("Invalid stream."));
            return YamlNode();
        }

        if (!parser.openStream(stream))
            return YamlNode();

        if (!parser.parse())
            return YamlNode();

        return parser.rootNode();
    }

    YamlNode yamlParseFile(const FileReaderPtr& reader, YamlErrorPtr& error)
    {
        YamlParser parser(error);

        if (!reader) {
            error.reset(new std::string("Invalid file."));
            return YamlNode();
        }

        FileInputStream stream(reader);
        if (!parser.openStream(&stream))
            return YamlNode();

        if (!parser.parse())
            return YamlNode();

        return parser.rootNode();
    }

    YamlNode yamlParseFile(const std::string& fileName, YamlErrorPtr& error)
    {
        YamlParser parser(error);

        FileReaderPtr reader = FileSystem::defaultFileSystem()->openFile(fileName);
        if (!reader) {
            std::stringstream ss;
            ss << "Unable to open file \"" << fileName << "\".";
            error.reset(new std::string(ss.str()));
            return YamlNode();
        }

        return yamlParseFile(reader, error);
    }
}
