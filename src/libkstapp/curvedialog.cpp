/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "curvedialog.h"

#include "dialogpage.h"

#include "curve.h"

#include "view.h"
#include "plotitem.h"
#include "tabwidget.h"
#include "mainwindow.h"
#include "application.h"
#include "plotrenderitem.h"

#include "defaultnames.h"
#include "datacollection.h"
#include "dataobjectcollection.h"
#include "document.h"
#include "objectstore.h"

#include <QPushButton>

namespace Kst {

CurveTab::CurveTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Curve"));

  _xError->setAllowEmptySelection(true);
  _yError->setAllowEmptySelection(true);

  _xMinusError->setAllowEmptySelection(true);
  _yMinusError->setAllowEmptySelection(true);

  _curvePlacement->setExistingPlots(Data::self()->plotList());

  connect(_xVector, SIGNAL(selectionChanged(QString)), this, SIGNAL(vectorsChanged()));
  connect(_yVector, SIGNAL(selectionChanged(QString)), this, SIGNAL(vectorsChanged()));

  connect(_xVector, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_yVector, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_xError, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_yError, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_xMinusError, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_yMinusError, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_curveAppearance, SIGNAL(modified()), this, SIGNAL(modified()));
}


CurveTab::~CurveTab() {
}


VectorPtr CurveTab::xVector() const {
  return _xVector->selectedVector();
}


void CurveTab::setXVector(VectorPtr vector) {
  _xVector->setSelectedVector(vector);
}


VectorPtr CurveTab::yVector() const {
  return _yVector->selectedVector();
}


void CurveTab::setYVector(VectorPtr vector) {
  _yVector->setSelectedVector(vector);
}


VectorPtr CurveTab::xError() const {
  return _xError->selectedVector();
}


void CurveTab::setXError(VectorPtr vector) {
  _xError->setSelectedVector(vector);
}


VectorPtr CurveTab::yError() const {
  return _yError->selectedVector();
}


void CurveTab::setYError(VectorPtr vector) {
  _yError->setSelectedVector(vector);
}


VectorPtr CurveTab::xMinusError() const {
  return _xMinusError->selectedVector();
}


void CurveTab::setXMinusError(VectorPtr vector) {
  _xMinusError->setSelectedVector(vector);
}


VectorPtr CurveTab::yMinusError() const {
  return _yMinusError->selectedVector();
}


void CurveTab::setYMinusError(VectorPtr vector) {
  _yMinusError->setSelectedVector(vector);
}


void CurveTab::setObjectStore(ObjectStore *store) {
  _xVector->setObjectStore(store);
  _yVector->setObjectStore(store);
  _xError->setObjectStore(store);
  _yError->setObjectStore(store);
  _xMinusError->setObjectStore(store);
  _yMinusError->setObjectStore(store);
}


void CurveTab::hidePlacementOptions() {
  _curvePlacement->setVisible(false);
  setMaximumHeight(400);
}



CurveAppearance* CurveTab::curveAppearance() const {
  return _curveAppearance;
}


CurvePlacement* CurveTab::curvePlacement() const {
  return _curvePlacement;
}


CurveDialog::CurveDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Curve"));
  else
    setWindowTitle(tr("New Curve"));

  _curveTab = new CurveTab(this);
  addDataTab(_curveTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  }

  connect(_curveTab, SIGNAL(vectorsChanged()), this, SLOT(updateButtons()));
  updateButtons();
}


CurveDialog::~CurveDialog() {
}


QString CurveDialog::tagString() const {
  return DataDialog::tagString();
}


void CurveDialog::configureTab(ObjectPtr object) {
  if (CurvePtr curve = kst_cast<Curve>(object)) {
    _curveTab->setXVector(curve->xVector());
    _curveTab->setYVector(curve->yVector());
    if (curve->hasXError()) {
      _curveTab->setXError(curve->xErrorVector());
    }
    if (curve->hasYError()) {
    _curveTab->setYError(curve->yErrorVector());
    }
    if (curve->hasXMinusError()) {
    _curveTab->setXMinusError(curve->xMinusErrorVector());
    }
    if (curve->hasYMinusError()) {
      _curveTab->setYMinusError(curve->yMinusErrorVector());
    }
    _curveTab->curveAppearance()->setColor(curve->color());
    _curveTab->curveAppearance()->setShowPoints(curve->hasPoints());
    _curveTab->curveAppearance()->setShowLines(curve->hasLines());
    _curveTab->curveAppearance()->setShowBars(curve->hasBars());
    _curveTab->curveAppearance()->setLineWidth(curve->lineWidth());
    _curveTab->curveAppearance()->setLineStyle(curve->lineStyle());
    _curveTab->curveAppearance()->setPointType(curve->pointType());
    _curveTab->curveAppearance()->setPointDensity(curve->pointDensity());
    _curveTab->curveAppearance()->setBarStyle(curve->barStyle());
    _curveTab->hidePlacementOptions();
  }
}


void CurveDialog::setVector(VectorPtr vector) {
  _curveTab->setYVector(vector);
}


void CurveDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_curveTab->xVector() && _curveTab->yVector());
}


ObjectPtr CurveDialog::createNewDataObject() const {
  Q_ASSERT(_document && _document->objectStore());
  CurvePtr curve = _document->objectStore()->createObject<Curve>(ObjectTag::fromString(tagString()));
#if 0
  CurvePtr curve = new Curve(_document->objectStore(),
                             ObjectTag::fromString(tagString()),
                             _curveTab->xVector(),
                             _curveTab->yVector(),
                             _curveTab->xError(),
                             _curveTab->yError(),
                             _curveTab->xMinusError(),
                             _curveTab->yMinusError(),
                             _curveTab->curveAppearance()->color());
#endif

  curve->setXVector(_curveTab->xVector());
  curve->setYVector(_curveTab->yVector());
  curve->setXError(_curveTab->xError());
  curve->setYError(_curveTab->yError());
  curve->setXMinusError(_curveTab->xMinusError());
  curve->setYMinusError(_curveTab->yMinusError());
  curve->setColor(_curveTab->curveAppearance()->color());
  curve->setHasPoints(_curveTab->curveAppearance()->showPoints());
  curve->setHasLines(_curveTab->curveAppearance()->showLines());
  curve->setHasBars(_curveTab->curveAppearance()->showBars());
  curve->setLineWidth(_curveTab->curveAppearance()->lineWidth());
  curve->setLineStyle(_curveTab->curveAppearance()->lineStyle());
  curve->setPointType(_curveTab->curveAppearance()->pointType());
  curve->setPointDensity(_curveTab->curveAppearance()->pointDensity());
  curve->setBarStyle(_curveTab->curveAppearance()->barStyle());

  curve->writeLock();
  curve->update(0);
  curve->unlock();

  PlotItem *plotItem = 0;
  switch (_curveTab->curvePlacement()->place()) {
  case CurvePlacement::NoPlot:
    break;
  case CurvePlacement::ExistingPlot:
    {
      plotItem = static_cast<PlotItem*>(_curveTab->curvePlacement()->existingPlot());
      break;
    }
  case CurvePlacement::NewPlot:
    {
      CreatePlotForCurve *cmd = new CreatePlotForCurve(
        _curveTab->curvePlacement()->createLayout(),
        _curveTab->curvePlacement()->appendToLayout());
      cmd->createItem();

      plotItem = static_cast<PlotItem*>(cmd->item());
      break;
    }
  default:
    break;
  }

  PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
  renderItem->addRelation(kst_cast<Relation>(curve));
  plotItem->update();

  return ObjectPtr(curve.data());
}


ObjectPtr CurveDialog::editExistingDataObject() const {
  if (CurvePtr curve = kst_cast<Curve>(dataObject())) {
    curve->writeLock();
    curve->setXVector(_curveTab->xVector());
    curve->setYVector(_curveTab->yVector());
    curve->setXError(_curveTab->xError());
    curve->setYError(_curveTab->yError());
    curve->setXMinusError(_curveTab->xMinusError());
    curve->setYMinusError(_curveTab->yMinusError());
    curve->setColor(_curveTab->curveAppearance()->color());
    curve->setHasPoints(_curveTab->curveAppearance()->showPoints());
    curve->setHasLines(_curveTab->curveAppearance()->showLines());
    curve->setHasBars(_curveTab->curveAppearance()->showBars());
    curve->setLineWidth(_curveTab->curveAppearance()->lineWidth());
    curve->setLineStyle(_curveTab->curveAppearance()->lineStyle());
    curve->setPointType(_curveTab->curveAppearance()->pointType());
    curve->setPointDensity(_curveTab->curveAppearance()->pointDensity());
    curve->setBarStyle(_curveTab->curveAppearance()->barStyle());

    curve->update(0);
    curve->unlock();
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
