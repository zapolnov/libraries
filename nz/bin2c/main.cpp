/*
 * Copyright (c) 2014 Nikolay Zapolnov (zapolnov@gmail.com).
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
#include "bin2c.h"
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <sstream>
#include <vector>

namespace
{
    struct InputFile
    {
        const char* name;
        const char* ident;
        bool compress;
    };
}

static bool isIdent(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_';
}

static bool readFile(const char* file, std::vector<char>& data)
{
    FILE* f = fopen(file, "rb");
    if (!f)
    {
        fprintf(stderr, "can't open file \"%s\": %s\n", file, strerror(errno));
        data.clear();
        return false;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (ferror(f) || size < 0)
    {
        fprintf(stderr, "can't determine size of file \"%s\": %s\n", file, strerror(errno));
        fclose(f);
        data.clear();
        return false;
    }

    data.resize((size_t)size);
    size_t bytesRead = fread(&data[0], 1, (size_t)size, f);
    if (ferror(f) || (long)bytesRead != size)
    {
        fprintf(stderr, "can't read file \"%s\": %s\n", file, strerror(errno));
        fclose(f);
        data.clear();
        return false;
    }

    fclose(f);

    return true;
}

static bool writeFile(const char* file, const void* data, size_t length, bool silent)
{
    FILE* f = fopen(file, "rb");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);
        if (!ferror(f) && size >= 0 && (size_t)size == length)
        {
            if (size == 0)
            {
                fclose(f);
                if (!silent)
                    printf("keeping %s\n", file);
                return true;
            }

            std::vector<char> buf((size_t)size);
            size_t bytesRead = fread(&buf[0], 1, (size_t)size, f);
            if (!ferror(f) && (long)bytesRead == size && !memcmp(data, &buf[0], length))
            {
                fclose(f);
                if (!silent)
                    printf("keeping %s\n", file);
                return true;
            }
        }
        fclose(f);
    }

    if (!silent)
        printf("writing %s\n", file);

    f = fopen(file, "wb");
    if (!f)
    {
        fprintf(stderr, "can't create file \"%s\": %s\n", file, strerror(errno));
        return false;
    }
    size_t bytesWritten = fwrite(data, 1, length, f);
    if (ferror(f) || bytesWritten != length)
    {
        fprintf(stderr, "can't write file \"%s\": %s\n", file, strerror(errno));
        fclose(f);
        return false;
    }
    fflush(f);
    if (ferror(f))
    {
        fprintf(stderr, "can't write file \"%s\": %s\n", file, strerror(errno));
        fclose(f);
        return false;
    }
    fclose(f);

    return true;
}

int main(int argc, char ** argv)
{
    const char* ident = NULL, *output = NULL;
    bool silent = false, compress = false;
    std::vector<InputFile> inputs;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            InputFile file;
            file.name = argv[i];
            file.compress = compress;
            file.ident = ident;
            inputs.push_back(file);
            ident = NULL;
        }
        else if (!strcmp(argv[i], "-c"))
            compress = true;
        else if (!strcmp(argv[i], "-nc"))
            compress = false;
        else if (!strcmp(argv[i], "-s"))
            silent = true;
        else if (!strcmp(argv[i], "-ns"))
            silent = false;
        else if (!strcmp(argv[i], "-o"))
        {
            if (i == argc - 1)
            {
                fprintf(stderr, "missing output file name after '-o'.\n");
                return 1;
            }
            output = argv[++i];
        }
        else if (!strcmp(argv[i], "-i"))
        {
            if (i == argc - 1)
            {
                fprintf(stderr, "missing identifier after '-i'.\n");
                return 1;
            }
            ident = argv[++i];
        }
    }

    if (!inputs.size())
    {
        fprintf(stderr, "missing input files.\n");
        return 1;
    }

    std::string outputName;
    if (!output)
    {
        if (inputs.size() != 1)
        {
            fprintf(stderr, "output file name was not specified on the command line.\n");
            return 1;
        }

        std::stringstream ss;
        for (const char* p = inputs[0].name; *p; ++p)
            ss << (*p == '.' ? '_' : *p);
        ss << ".cpp";
        outputName = ss.str();
        output = outputName.c_str();
    }

    std::stringstream result;
    for (std::vector<InputFile>::const_iterator it = inputs.begin(); it != inputs.end(); ++it)
    {
        const char* ident = it->ident;
        std::string identName;
        if (!ident)
        {
            std::stringstream ss;
            ss << "file_";
            for (const char* p = it->name; *p; ++p)
                ss << (!isIdent(*p) ? '_' : *p);
            identName = ss.str();
            ident = identName.c_str();
        }

        std::vector<char> data;
        if (!readFile(it->name, data))
            return 1;

        if (!bin2c(result, ident, &data[0], data.size(), it->compress))
        {
            fprintf(stderr, "unable to compress file \"%s\".\n", it->name);
            return 1;
        }
    }

    std::string data = result.str();
    if (!writeFile(output, data.data(), data.length(), silent))
        return 1;

    return 0;
}
