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
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace Z
{
    class YamlNode
    {
    public:
        enum Type
        {
            Invalid,
            Plain,
            SingleQuoted,
            DoubleQuoted,
            Literal,
            Folded,
            Sequence,
            Mapping,
        };

        enum BooleanValue { False = 0, True = 1, Indeterminate = -1 };
        typedef std::vector<YamlNode> SequenceValue;
        typedef std::unordered_multimap<std::string, YamlNode> MappingValue;

        YamlNode() = default;
        ~YamlNode() = default;

        static YamlNode plain(const std::string& text, int line, int column);
        static YamlNode plain(std::string&& text, int line, int column);
        static YamlNode singleQuoted(const std::string& text, int line, int column);
        static YamlNode singleQuoted(std::string&& text, int line, int column);
        static YamlNode doubleQuoted(const std::string& text, int line, int column);
        static YamlNode doubleQuoted(std::string&& text, int line, int column);
        static YamlNode literal(const std::string& text, int line, int column);
        static YamlNode literal(std::string&& text, int line, int column);
        static YamlNode folded(const std::string& text, int line, int column);
        static YamlNode folded(std::string&& text, int line, int column);
        static YamlNode sequence(const SequenceValue& items, int line, int column);
        static YamlNode sequence(SequenceValue&& items, int line, int column);
        static YamlNode sequence(SequenceValue*& items, int line, int column);
        static YamlNode mapping(const MappingValue& items, int line, int column);
        static YamlNode mapping(MappingValue&& items, int line, int column);
        static YamlNode mapping(MappingValue*& items, int line, int column);

        Type type() const;
        int line() const;
        int column() const;

        bool isInvalid() const;
        bool isText() const;
        bool isSequence() const;
        bool isMapping() const;

        BooleanValue toBool() const;
        int toInt() const;
        size_t toSizeT() const;
        double toDouble() const;
        const std::string& toString() const;
        const SequenceValue& toSequence() const;
        const MappingValue& toMapping() const;

    private:
        struct Data;
        struct TextData;
        struct SequenceData;
        struct MappingData;

        std::shared_ptr<Data> m_Data;
    };
}
