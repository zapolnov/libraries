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
#include "YamlMappingVisitor.h"
#include "utility/debug.h"
#include <sstream>

namespace Z
{
    YamlMappingVisitor::YamlMappingVisitor(const std::string& fileName, const YamlNode& node)
        : m_FileName(fileName)
        , m_Node(node)
    {
    }

    YamlMappingVisitor::~YamlMappingVisitor()
    {
    }

    void YamlMappingVisitor::print(const YamlNode& node, const std::string& message)
    {
        Z_LOG("Unable to parse file \"" << m_FileName << "\": at line " << node.line() << ": " << message);
    }

    const std::string& YamlMappingVisitor::baseDir()
    {
        if (!m_BaseDir) {
            size_t i = m_FileName.rfind('/');
            m_BaseDir.reset(new std::string(i == std::string::npos ? std::string() : m_FileName.substr(0, i + 1)));
        }
        return *m_BaseDir;
    }

    bool YamlMappingVisitor::nodeToBool(const YamlNode& node, bool& result) const
    {
        auto value = node.toBool();
        if (value == Z::YamlNode::Indeterminate)
            return false;
        result = (value == Z::YamlNode::True);
        return true;
    }

    std::string YamlMappingVisitor::nodeToPath(const FileSystemPtr& fileSystem, const YamlNode& node)
    {
        std::string value = node.toString();
        size_t length = value.length();
        if (length > 0 && value[0] == '/')
            return value.substr(1);
        else {
            std::string localFileName = baseDir() + value;
            if (fileSystem->fileExists(localFileName))
                return localFileName;
            return value;
        }
    }

    bool YamlMappingVisitor::visit()
    {
        for (const auto& option : m_Node.toMapping())
        {
            Result result = visitKeyValue(option.first, option.second);
            switch (result)
            {
            case Success:
                continue;

            case UnknownOption: {
                std::stringstream ss;
                ss << "unknown option \"" << option.first << "\".";
                print(option.second, ss.str());
                return false;
                }

            case InvalidValue: {
                std::stringstream ss;
                ss << "invalid value for option \"" << option.first << "\".";
                print(option.second, ss.str());
                return false;
                }

            default:
                Z_ASSERT(false);
                print(option.second, "error.");
                return false;
            }
        }

        return true;
    }
}
