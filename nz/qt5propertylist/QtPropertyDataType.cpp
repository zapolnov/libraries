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
#include "QtPropertyDataType.h"
#include "QtPropertyListItem.h"
#include <QStyle>
#include <QStyleOPtionButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <memory>
#include <limits>
#include <unordered_map>
#include <cassert>

namespace Z
{
    namespace
    {
        template <class TYPE> static void initStyleOption(TYPE& option, const QStyleOptionViewItem& srcOption)
        {
            option.direction = srcOption.direction;
            option.fontMetrics = srcOption.fontMetrics;
            option.palette = srcOption.palette;
            option.rect = srcOption.rect;
            option.state = srcOption.state;
        }

        struct BoolType : public QtPropertyDataType
        {
            QVariant formatForDisplay(const QVariant& data) const override {
                return QVariant();
            }

            void paint(QPainter* painter, const QVariant& data,
                QStyle* style, const QStyleOptionViewItem& option) const override
            {
                QStyleOptionButton buttonOption;
                initStyleOption(buttonOption, option);
                buttonOption.state &= ~(QStyle::State_Sunken | QStyle::State_NoChange);
                if (data.toBool())
                    buttonOption.state |= QStyle::State_On;
                style->drawControl(QStyle::CE_CheckBox, &buttonOption, painter, nullptr);
            }

            QWidget* createEditor(QWidget* parent) const override {
                auto checkBox = new QCheckBox(parent);
                checkBox->setStyleSheet("background-color: #8e2dc5");
                return checkBox;
            }

            QVariant getEditorData(QWidget* editor) const override {
                auto checkBox = qobject_cast<QCheckBox*>(editor);
                return (checkBox ? QVariant(checkBox->isChecked()) : QVariant());
            }

            void setEditorData(QWidget* editor, const QVariant& data) const override {
                auto checkBox = qobject_cast<QCheckBox*>(editor);
                if (checkBox)
                    checkBox->setChecked(data.toBool());
            }
        };

        struct IntType : public QtPropertyDataType {
            QVariant formatForDisplay(const QVariant& data) const override {
                return data.toInt();
            }

            QWidget* createEditor(QWidget* parent) const override {
                auto spinBox = new QSpinBox(parent);
                spinBox->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
                return spinBox;
            }

            QVariant getEditorData(QWidget* editor) const override {
                auto spinBox = qobject_cast<QSpinBox*>(editor);
                return (spinBox ? QVariant(int(spinBox->value())) : QVariant());
            }

            void setEditorData(QWidget* editor, const QVariant& data) const override {
                auto spinBox = qobject_cast<QSpinBox*>(editor);
                if (spinBox)
                    spinBox->setValue(data.toInt());
            }
        };

        struct FloatType : public QtPropertyDataType {
            const int decimals;
            explicit FloatType(int d) : decimals(d) {}

            QVariant formatForDisplay(const QVariant& data) const override {
                return QString::number(data.toFloat(), 'f', decimals);
            }

            QWidget* createEditor(QWidget* parent) const override {
                auto doubleSpinBox = new QDoubleSpinBox(parent);
                doubleSpinBox->setRange(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
                doubleSpinBox->setDecimals(decimals);
                return doubleSpinBox;
            }

            QVariant getEditorData(QWidget* editor) const override {
                auto doubleSpinBox = qobject_cast<QDoubleSpinBox*>(editor);
                return (doubleSpinBox ? QVariant(float(doubleSpinBox->value())) : QVariant());
            }

            void setEditorData(QWidget* editor, const QVariant& data) const override {
                auto doubleSpinBox = qobject_cast<QDoubleSpinBox*>(editor);
                if (doubleSpinBox)
                    doubleSpinBox->setValue(data.toFloat());
            }
        };

        struct StringType : public QtPropertyDataType {
            QVariant formatForDisplay(const QVariant& data) const override {
                return data.toString();
            }

            QWidget* createEditor(QWidget* parent) const override {
                auto lineEdit = new QLineEdit(parent);
                return lineEdit;
            }

            QVariant getEditorData(QWidget* editor) const override {
                auto lineEdit = qobject_cast<QLineEdit*>(editor);
                return (lineEdit ? QVariant(lineEdit->text()) : QVariant());
            }

            void setEditorData(QWidget* editor, const QVariant& data) const override {
                auto lineEdit = qobject_cast<QLineEdit*>(editor);
                if (lineEdit)
                    lineEdit->setText(data.toString());
            }
        };

        static const BoolType boolType = {};
        static const IntType intType = {};
        static const StringType stringType = {};
        static std::unordered_map<int, std::unique_ptr<FloatType>> floatTypes;
    };

    const QtPropertyDataType* const QtPropertyDataType::Bool = &boolType;
    const QtPropertyDataType* const QtPropertyDataType::Int = &intType;
    const QtPropertyDataType* const QtPropertyDataType::String = &stringType;

    const QtPropertyDataType* QtPropertyDataType::floatType(int precision)
    {
        assert(precision >= 0);

        std::unique_ptr<FloatType>& ptr = floatTypes[precision];
        if (!ptr)
            ptr.reset(new FloatType(precision));

        return ptr.get();
    }

    QVariant QtPropertyDataType::formatForDisplay(const QVariant& data) const { return data; }
    void QtPropertyDataType::paint(QPainter*, const QVariant& data, QStyle*, const QStyleOptionViewItem&) const {}
    QWidget* QtPropertyDataType::createEditor(QWidget* parent) const { return new QWidget(parent); }
    QVariant QtPropertyDataType::getEditorData(QWidget*) const { return QVariant(); }
    void QtPropertyDataType::setEditorData(QWidget*, const QVariant&) const {}
}
