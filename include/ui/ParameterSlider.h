#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTimer>
#include <QString>

namespace ArchMaths {

class ParameterSlider : public QWidget {
    Q_OBJECT

public:
    explicit ParameterSlider(const QString& name, double value = 1.0,
                             double minVal = -10.0, double maxVal = 10.0,
                             QWidget* parent = nullptr);

    QString name() const { return name_; }
    double value() const { return value_; }
    void setValue(double val);
    bool isPlaying() const { return playing_; }

signals:
    void valueChanged(const QString& name, double value);

private slots:
    void onSliderChanged(int val);
    void onSpinBoxChanged(double val);
    void onPlayClicked();
    void onAnimationTick();

private:
    void setupUI();
    void expandRangeIfNeeded(double val);

    QString name_;
    double value_;
    double minVal_;
    double maxVal_;
    bool playing_ = false;
    double animPhase_ = 0.0;

    QLabel* nameLabel_;
    QSlider* slider_;
    QDoubleSpinBox* spinBox_;
    QPushButton* playBtn_;
    QTimer* animTimer_;
};

} // namespace ArchMaths
