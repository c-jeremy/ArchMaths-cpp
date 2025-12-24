#include "ui/ParameterSlider.h"
#include <QHBoxLayout>
#include <cmath>

namespace ArchMaths {

ParameterSlider::ParameterSlider(const QString& name, double value,
                                 double minVal, double maxVal, QWidget* parent)
    : QWidget(parent)
    , name_(name)
    , value_(value)
    , minVal_(minVal)
    , maxVal_(maxVal)
{
    setupUI();
    setValue(value);
}

void ParameterSlider::setupUI() {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 2, 0, 2);
    layout->setSpacing(5);

    nameLabel_ = new QLabel(name_ + ":", this);
    nameLabel_->setFixedWidth(20);
    nameLabel_->setStyleSheet("QLabel { font-weight: bold; color: #555; }");

    playBtn_ = new QPushButton("▶", this);
    playBtn_->setFixedSize(24, 24);
    playBtn_->setStyleSheet("QPushButton { font-size: 10px; }");
    connect(playBtn_, &QPushButton::clicked, this, &ParameterSlider::onPlayClicked);

    slider_ = new QSlider(Qt::Horizontal, this);
    slider_->setRange(0, 1000);
    slider_->setFixedHeight(20);
    connect(slider_, &QSlider::valueChanged, this, &ParameterSlider::onSliderChanged);

    spinBox_ = new QDoubleSpinBox(this);
    spinBox_->setRange(-1e9, 1e9);  // Wide range to allow any input
    spinBox_->setDecimals(2);
    spinBox_->setSingleStep(0.1);
    spinBox_->setFixedWidth(70);
    connect(spinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ParameterSlider::onSpinBoxChanged);

    animTimer_ = new QTimer(this);
    connect(animTimer_, &QTimer::timeout, this, &ParameterSlider::onAnimationTick);

    layout->addWidget(nameLabel_);
    layout->addWidget(playBtn_);
    layout->addWidget(slider_, 1);
    layout->addWidget(spinBox_);
}

void ParameterSlider::setValue(double val) {
    value_ = std::clamp(val, minVal_, maxVal_);

    slider_->blockSignals(true);
    int sliderVal = static_cast<int>((value_ - minVal_) / (maxVal_ - minVal_) * 1000);
    slider_->setValue(sliderVal);
    slider_->blockSignals(false);

    spinBox_->blockSignals(true);
    spinBox_->setValue(value_);
    spinBox_->blockSignals(false);
}

void ParameterSlider::onSliderChanged(int val) {
    value_ = minVal_ + (maxVal_ - minVal_) * val / 1000.0;

    spinBox_->blockSignals(true);
    spinBox_->setValue(value_);
    spinBox_->blockSignals(false);

    emit valueChanged(name_, value_);
}

void ParameterSlider::onSpinBoxChanged(double val) {
    expandRangeIfNeeded(val);
    value_ = val;

    slider_->blockSignals(true);
    int sliderVal = static_cast<int>((value_ - minVal_) / (maxVal_ - minVal_) * 1000);
    slider_->setValue(sliderVal);
    slider_->blockSignals(false);

    emit valueChanged(name_, value_);
}

void ParameterSlider::onPlayClicked() {
    playing_ = !playing_;
    playBtn_->setText(playing_ ? "⏸" : "▶");
    if (playing_) {
        animTimer_->start(33); // ~30 FPS
    } else {
        animTimer_->stop();
    }
}

void ParameterSlider::onAnimationTick() {
    animPhase_ += 0.05;
    double t = (std::sin(animPhase_) + 1.0) / 2.0; // 0 to 1
    double newVal = minVal_ + t * (maxVal_ - minVal_);
    setValue(newVal);
    emit valueChanged(name_, newVal);
}

void ParameterSlider::expandRangeIfNeeded(double val) {
    bool changed = false;
    if (val < minVal_) {
        minVal_ = val - std::abs(val) * 0.1 - 1.0;  // Add 10% margin
        changed = true;
    }
    if (val > maxVal_) {
        maxVal_ = val + std::abs(val) * 0.1 + 1.0;  // Add 10% margin
        changed = true;
    }
    if (changed) {
        // Update slider position for new range
        slider_->blockSignals(true);
        int sliderVal = static_cast<int>((value_ - minVal_) / (maxVal_ - minVal_) * 1000);
        slider_->setValue(sliderVal);
        slider_->blockSignals(false);
    }
}

} // namespace ArchMaths
