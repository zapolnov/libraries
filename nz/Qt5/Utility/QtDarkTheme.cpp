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
#include "QtDarkTheme.h"
#include <QApplication>
#include <QPalette>
#include <QStyleFactory>

namespace Z
{
    QColor darkThemeBaseColor = { 43, 43, 43 };
    QColor darkThemeBackgroundColor = { 53, 53, 53 };
    QColor darkThemeForegroundColor = { 230, 230, 230 };
    QColor darkThemeForegroundDisabledColor = { 128, 128, 128 };
    QColor darkThemeHighlightColor = { 142, 45, 197 };
    QColor darkThemeBrightColor = { 255, 255, 255 };

    void qtSetDarkTheme()
    {
        qApp->setStyle(QStyleFactory::create("Fusion"));

        QPalette pal = qApp->palette();
        pal.setColor(QPalette::Window, darkThemeBackgroundColor);
        pal.setColor(QPalette::WindowText, darkThemeForegroundColor);
        pal.setColor(QPalette::Disabled, QPalette::WindowText, darkThemeForegroundDisabledColor);
        pal.setColor(QPalette::Base, darkThemeBaseColor);
        pal.setColor(QPalette::AlternateBase, darkThemeBackgroundColor);
        pal.setColor(QPalette::ToolTipBase, darkThemeBackgroundColor);
        pal.setColor(QPalette::ToolTipText, darkThemeForegroundColor);
        pal.setColor(QPalette::Text, darkThemeForegroundColor);
        pal.setColor(QPalette::Disabled, QPalette::Text, darkThemeForegroundDisabledColor);
        pal.setColor(QPalette::Button, darkThemeBackgroundColor);
        pal.setColor(QPalette::ButtonText, darkThemeForegroundColor);
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, darkThemeForegroundDisabledColor);
        pal.setColor(QPalette::BrightText, darkThemeBrightColor);
        pal.setColor(QPalette::Highlight, darkThemeHighlightColor);
        pal.setColor(QPalette::HighlightedText, darkThemeForegroundColor);
        pal.setColor(QPalette::Disabled, QPalette::HighlightedText, darkThemeForegroundDisabledColor);
        pal.setColor(QPalette::Disabled, QPalette::Light, darkThemeForegroundDisabledColor.darker());
        qApp->setPalette(pal);
    }
}
