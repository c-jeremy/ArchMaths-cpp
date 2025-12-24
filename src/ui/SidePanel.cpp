#include "ui/SidePanel.h"
#include "ui/EntryWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

namespace ArchMaths {

SidePanel::SidePanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void SidePanel::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // 精度控制
    QHBoxLayout* precisionLayout = new QHBoxLayout();
    precisionLabel_ = new QLabel("精度: 1.0x", this);
    precisionSlider_ = new QSlider(Qt::Horizontal, this);
    precisionSlider_->setRange(1, 40);  // 0.1x to 4.0x
    precisionSlider_->setValue(10);     // Default 1.0x
    precisionLayout->addWidget(precisionLabel_);
    precisionLayout->addWidget(precisionSlider_);
    connect(precisionSlider_, &QSlider::valueChanged, this, [this](int value) {
        double multiplier = value / 10.0;
        precisionLabel_->setText(QString("精度: %1x").arg(multiplier, 0, 'f', 1));
        emit precisionChanged(multiplier);
    });
    mainLayout->addLayout(precisionLayout);

    // 滚动区域
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* scrollContent = new QWidget();
    entriesLayout_ = new QVBoxLayout(scrollContent);
    entriesLayout_->setContentsMargins(0, 0, 0, 0);
    entriesLayout_->setSpacing(5);
    entriesLayout_->addStretch();

    scrollArea_->setWidget(scrollContent);

    // 添加按钮
    addButton_ = new QPushButton("+ 添加表达式", this);
    addButton_->setMinimumHeight(40);
    connect(addButton_, &QPushButton::clicked, this, &SidePanel::addEntryRequested);

    mainLayout->addWidget(scrollArea_);
    mainLayout->addWidget(addButton_);
}

void SidePanel::addEntry(const PlotEntry& entry) {
    int index = static_cast<int>(entryWidgets_.size());
    EntryWidget* widget = new EntryWidget(index, entry, this);

    connect(widget, &EntryWidget::expressionChanged,
            this, &SidePanel::entryChanged);
    connect(widget, &EntryWidget::deleteRequested,
            this, &SidePanel::entryDeleted);
    connect(widget, &EntryWidget::visibilityChanged,
            this, &SidePanel::entryVisibilityChanged);
    connect(widget, &EntryWidget::parameterChanged,
            this, &SidePanel::entryParameterChanged);
    connect(widget, &EntryWidget::colorClicked,
            this, &SidePanel::entryColorChanged);

    entryWidgets_.push_back(widget);

    // Insert before stretch
    entriesLayout_->insertWidget(entriesLayout_->count() - 1, widget);
}

void SidePanel::updateEntry(int index, const PlotEntry& entry) {
    if (index >= 0 && index < static_cast<int>(entryWidgets_.size())) {
        entryWidgets_[index]->setEntry(entry);
    }
}

void SidePanel::removeEntry(int index) {
    if (index >= 0 && index < static_cast<int>(entryWidgets_.size())) {
        EntryWidget* widget = entryWidgets_[index];
        entryWidgets_.erase(entryWidgets_.begin() + index);
        entriesLayout_->removeWidget(widget);
        widget->deleteLater();

        // 更新后续widget的索引
        for (int i = index; i < static_cast<int>(entryWidgets_.size()); ++i) {
            entryWidgets_[i]->setIndex(i);
        }
    }
}

void SidePanel::clear() {
    for (auto* widget : entryWidgets_) {
        entriesLayout_->removeWidget(widget);
        widget->deleteLater();
    }
    entryWidgets_.clear();
}

} // namespace ArchMaths
