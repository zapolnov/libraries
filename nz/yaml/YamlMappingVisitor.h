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

#pragma once
#include "YamlNode.h"
#include "utility/FileSystem.h"
#include <memory>
#include <string>
#include <initializer_list>
#include <functional>
#include <unordered_map>

namespace Z
{
    class YamlMappingVisitor
    {
    public:
        YamlMappingVisitor(const std::string& fileName, const YamlNode& node);
        ~YamlMappingVisitor();

        const std::string& baseDir();

        void print(const YamlNode& node, const std::string& message);

        bool nodeToBool(const YamlNode& node, bool& result) const;
        std::string nodeToPath(const FileSystemPtr& fileSystem, const YamlNode& node);

        template <class TYPE> using EnumValues = std::initializer_list<std::pair<std::string, TYPE>>;
        template <class TYPE> bool nodeToEnum(const YamlNode& node, TYPE& result, const EnumValues<TYPE>& values) const
        {
            const std::string& str = node.toString();
            for (const auto& value : values) {
                if (str == value.first) {
                    result = value.second;
                    return true;
                }
            }
            return false;
        }

        bool visit();

    protected:
        enum Result
        {
            Success = 0,
            UnknownOption,
            InvalidValue,
        };

        virtual Result visitKeyValue(const std::string& key, const YamlNode& node) = 0;

    private:
        std::string m_FileName;
        std::unique_ptr<std::string> m_BaseDir;
        YamlNode m_Node;
    };

    template <class CONTEXT> class YamlMappingVisitorEx : public YamlMappingVisitor
    {
    public:
        using HandlerMap = std::unordered_map<std::string,
            std::function<bool(CONTEXT*, YamlMappingVisitorEx*, const YamlNode&)>>;

        YamlMappingVisitorEx(CONTEXT* context, const std::string& fileName,
                const YamlNode& node, const HandlerMap& handlers)
            : YamlMappingVisitor(fileName, node)
            , m_Context(context)
            , m_Handlers(handlers)
        {
        }

    protected:
        Result visitKeyValue(const std::string& key, const YamlNode& node) override
        {
            auto it = m_Handlers.find(key);
            if (it == m_Handlers.end())
                return UnknownOption;

            if (!it->second(m_Context, this, node))
                return InvalidValue;

            return Success;
        }

    private:
        CONTEXT* m_Context;
        const HandlerMap& m_Handlers;
    };
}
