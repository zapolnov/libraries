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
#include "YamlNode.h"
#include <cctype>
#include <cassert>

namespace Z
{
    struct YamlNode::Data
    {
        Type type;
        int line;
        int column;

        Data(Type t, int l, int c) : type(t), line(l), column(c) {}
    };

    struct YamlNode::TextData : public Data
    {
        std::string text;

        TextData(Type t, const std::string& s, int l, int c) : Data(t, l, c), text(s) {}
        TextData(Type t, std::string&& s, int l, int c) : Data(t, l, c), text(std::move(s)) {}
    };

    struct YamlNode::SequenceData : public Data
    {
        SequenceValue items;

        SequenceData(int l, int c) : Data(Sequence, l, c) {}
        SequenceData(const SequenceValue& a, int l, int c) : Data(Sequence, l, c), items(a) {}
        SequenceData(SequenceValue&& a, int l, int c) : Data(Sequence, l, c), items(std::move(a)) {}
    };

    struct YamlNode::MappingData : public Data
    {
        MappingValue items;

        MappingData(int l, int c) : Data(Mapping, l, c) {}
        MappingData(const MappingValue& d, int l, int c) : Data(Mapping, l, c), items(d) {}
        MappingData(MappingValue&& d, int l, int c) : Data(Mapping, l, c), items(std::move(d)) {}
    };


    static const std::string g_EmptyString;
    static const YamlNode::MappingValue g_EmptyMapping;
    static const YamlNode::SequenceValue g_EmptySequence;
    static const std::unordered_map<std::string, YamlNode::BooleanValue> g_StringToBoolean = {
        { "true", YamlNode::True },
        { "false", YamlNode::False },
        { "on", YamlNode::True },
        { "off", YamlNode::False },
        { "yes", YamlNode::True },
        { "no", YamlNode::False },
        { "enable", YamlNode::True },
        { "disable", YamlNode::False },
        { "enabled", YamlNode::True },
        { "disabled", YamlNode::False },
    };


    YamlNode YamlNode::plain(const std::string& text, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<TextData>(Plain, text, line, column);
        return result;
    }

    YamlNode YamlNode::plain(std::string&& text, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<TextData>(Plain, std::move(text), line, column);
        return result;
    }

    YamlNode YamlNode::singleQuoted(const std::string& text, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<TextData>(SingleQuoted, text, line, column);
        return result;
    }

    YamlNode YamlNode::singleQuoted(std::string&& text, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<TextData>(SingleQuoted, std::move(text), line, column);
        return result;
    }

    YamlNode YamlNode::doubleQuoted(const std::string& text, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<TextData>(DoubleQuoted, text, line, column);
        return result;
    }

    YamlNode YamlNode::doubleQuoted(std::string&& text, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<TextData>(DoubleQuoted, std::move(text), line, column);
        return result;
    }

    YamlNode YamlNode::literal(const std::string& text, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<TextData>(Literal, text, line, column);
        return result;
    }

    YamlNode YamlNode::literal(std::string&& text, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<TextData>(Literal, std::move(text), line, column);
        return result;
    }

    YamlNode YamlNode::folded(const std::string& text, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<TextData>(Folded, text, line, column);
        return result;
    }

    YamlNode YamlNode::folded(std::string&& text, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<TextData>(Folded, std::move(text), line, column);
        return result;
    }

    YamlNode YamlNode::sequence(const SequenceValue& items, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<SequenceData>(items, line, column);
        return result;
    }

    YamlNode YamlNode::sequence(SequenceValue&& items, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<SequenceData>(std::move(items), line, column);
        return result;
    }

    YamlNode YamlNode::sequence(SequenceValue*& items, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<SequenceData>(line, column);
        items = &static_cast<SequenceData&>(*result.m_Data).items;
        return result;
    }

    YamlNode YamlNode::mapping(const MappingValue& items, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<MappingData>(items, line, column);
        return result;
    }

    YamlNode YamlNode::mapping(MappingValue&& items, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<MappingData>(std::move(items), line, column);
        return result;
    }

    YamlNode YamlNode::mapping(MappingValue*& items, int line, int column)
    {
        YamlNode result;
        result.m_Data = std::make_shared<MappingData>(line, column);
        items = &static_cast<MappingData&>(*result.m_Data).items;
        return result;
    }

    YamlNode::Type YamlNode::type() const
    {
        return (m_Data ? m_Data->type : Invalid);
    }

    int YamlNode::line() const
    {
        return (m_Data ? m_Data->line : 0);
    }

    int YamlNode::column() const
    {
        return (m_Data ? m_Data->column : 0);
    }

    bool YamlNode::isInvalid() const
    {
        return (!m_Data || m_Data->type == Invalid);
    }

    bool YamlNode::isText() const
    {
        if (!m_Data)
            return false;

        switch (m_Data->type)
        {
        case YamlNode::Invalid: return false;
        case YamlNode::Plain: return true;
        case YamlNode::SingleQuoted: return true;
        case YamlNode::DoubleQuoted: return true;
        case YamlNode::Literal: return true;
        case YamlNode::Folded: return true;
        case YamlNode::Sequence: return false;
        case YamlNode::Mapping: return false;
        }

        assert(false);
        return false;
    }

    bool YamlNode::isSequence() const
    {
        return (m_Data && m_Data->type == Sequence);
    }

    bool YamlNode::isMapping() const
    {
        return (m_Data && m_Data->type == Mapping);
    }

    YamlNode::BooleanValue YamlNode::toBool() const
    {
        if (isText())
        {
            std::string text = static_cast<TextData&>(*m_Data).text;
            for (auto& ch : text)
                ch = tolower(ch);

            auto it = g_StringToBoolean.find(text);
            if (it != g_StringToBoolean.end())
                return it->second;
        }

        return Indeterminate;
    }

    int YamlNode::toInt() const
    {
        if (isText())
        {
            const std::string& text = static_cast<TextData&>(*m_Data).text;
            size_t pos = std::string::npos;
            int value = std::stoi(text, &pos, 0);
            if (pos == text.length())
                return value;
        }

        return 0;
    }

    size_t YamlNode::toSizeT() const
    {
        if (isText())
        {
            const std::string& text = static_cast<TextData&>(*m_Data).text;
            size_t pos = std::string::npos;

            size_t value;
            if (sizeof(size_t) == sizeof(unsigned long))
                value = size_t(std::stoul(text, &pos, 0));
            else if (sizeof(size_t) == sizeof(unsigned long long))
                value = size_t(std::stoull(text, &pos, 0));
            else
            {
                static_assert(
                    sizeof(size_t) == sizeof(unsigned long) ||
                    sizeof(size_t) == sizeof(unsigned long long),
                    "unsupported sizeof(size_t).");
            }

            if (pos == text.length())
                return value;
        }

        return 0U;
    }

    double YamlNode::toDouble() const
    {
        if (isText())
        {
            const std::string& text = static_cast<TextData&>(*m_Data).text;
            size_t pos = std::string::npos;
            double value = std::stod(text, &pos);
            if (pos == text.length())
                return value;
        }

        return 0.0;
    }

    const std::string& YamlNode::toString() const
    {
        return (isText() ? static_cast<TextData&>(*m_Data).text : g_EmptyString);
    }

    const YamlNode::SequenceValue& YamlNode::toSequence() const
    {
        if (m_Data && m_Data->type == Sequence)
            return static_cast<SequenceData&>(*m_Data).items;
        return g_EmptySequence;
    }

    const YamlNode::MappingValue& YamlNode::toMapping() const
    {
        if (m_Data && m_Data->type == Mapping)
            return static_cast<MappingData&>(*m_Data).items;
        return g_EmptyMapping;
    }
}
