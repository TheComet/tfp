#pragma once

#include "tfp/views/Tool.hpp"

class DPSFGView;

class DPSFG : public tfp::Tool
{
public:
    explicit DPSFG(QWidget* parent=NULL);

protected:
    virtual void onSetSystem() override;
    virtual void onSystemStructureChanged() override {}
    virtual void onSystemParametersChanged() override {}
    virtual void replot() override {}
    virtual void autoScale() override {}

private:
    DPSFGView* dpsfgView_;
};
