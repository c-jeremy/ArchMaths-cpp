#pragma once

#include <cstring>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <vector>
#include "math/MathTypes.h"

namespace ArchMaths {

class ParameterSlider;

class EntryWidget : public QWidget {
    Q_OBJECT

public:
    explicit EntryWidget(int index, const PlotEntry& entry, QWidget* parent = nullptr);

    void setEntry(const PlotEntry& entry);
    int getIndex() const { return index_; }
    void setIndex(int index) { index_ = index; }

signals:
    void expressionChanged(int index, const QString& expression);
    void deleteRequested(int index);
    void visibilityChanged(int index, bool visible);
    void colorClicked(int index, const Color& color);
    void parameterChanged(int index, const QString& name, double value);

private slots:
    void onExpressionEdited();
    void onDeleteClicked();
    void onVisibilityToggled();
    void onColorClicked();
    void onParameterValueChanged(const QString& name, double value);

private:
    void setupUI();
    void updateColorButton();
    void updateParameterSliders(const std::vector<ParameterInfo>& parameters);

    int index_;
    PlotEntry entry_;

    QVBoxLayout* mainLayout_;
    QWidget* slidersContainer_;
    QVBoxLayout* slidersLayout_;
    QLineEdit* expressionEdit_;
    QPushButton* colorButton_;
    QPushButton* visibilityButton_;
    QPushButton* deleteButton_;
    QLabel* errorLabel_;
    std::vector<ParameterSlider*> parameterSliders_;
};

} // namespace ArchMaths
