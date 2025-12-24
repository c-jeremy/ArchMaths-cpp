#pragma once

#include <cstring>
#include <QMainWindow>
#include <QSplitter>
#include <memory>
#include <set>
#include "rendering/GLCanvas.h"
#include "math/ExpressionParser.h"
#include "math/ExpressionEvaluator.h"
#include "math/MathTypes.h"

namespace ArchMaths {

class SidePanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onAddEntry();
    void onEntryChanged(int index, const QString& expression);
    void onEntryDeleted(int index);
    void onViewChanged(QPointF offset, double scale);
    void onParameterChanged(int index, const QString& name, double value);
    void onEntryVisibilityChanged(int index, bool visible);
    void onEntryColorChanged(int index, const Color& color);
    void recalculateAll();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void connectSignals();

    void parseAndCompileEntry(PlotEntry& entry);
    void calculatePlotData(PlotEntry& entry);
    void calculatePlotData3D(PlotEntry& entry);
    void extractParameters(PlotEntry& entry);
    void collectVariables(const ExprNodePtr& node, std::set<std::string>& vars);
    bool containsVariable(const ExprNodePtr& node, const std::string& varName);

    // 3D mesh generation
    void generateSurfaceMesh(PlotEntry& entry, const std::vector<std::vector<double>>& zGrid,
                             const std::vector<double>& xVals, const std::vector<double>& yVals);
    void generateImplicit3DMesh(PlotEntry& entry, const std::vector<double>& field,
                                int nx, int ny, int nz,
                                double xMin, double xMax,
                                double yMin, double yMax,
                                double zMin, double zMax);

    // UI components
    QSplitter* splitter_;
    GLCanvas* canvas_;
    SidePanel* sidePanel_;

    // Math engine
    std::unique_ptr<ExpressionParser> parser_;
    std::unique_ptr<ExpressionEvaluator> evaluator_;
    UserFunctionRegistry userFunctions_;

    // Data
    std::vector<PlotEntry> entries_;
    VariableContext variables_;
    double precisionMultiplier_ = 1.0;

public slots:
    void setPrecisionMultiplier(double multiplier);
};

} // namespace ArchMaths
