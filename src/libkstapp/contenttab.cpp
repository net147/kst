/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contenttab.h"

#include "objectstore.h"
#include "relation.h"
#include "dialoglauncher.h"
#include "geticon.h"

#include <qdebug.h>

namespace Kst {

ContentTab::ContentTab(QWidget *parent, ObjectStore *store)
  : DialogTab(parent), _store(store) {

  setupUi(this);

  _up->setIcon(KstGetIcon("kst_uparrow"));
  _down->setIcon(KstGetIcon("kst_downarrow"));
  _add->setIcon(KstGetIcon("kst_rightarrow"));
  _remove->setIcon(KstGetIcon("kst_leftarrow"));
  _up->setToolTip(tr("Raise in plot order: Alt+Up"));
  _down->setToolTip(tr("Lower in plot order: Alt+Down"));
  _add->setToolTip(tr("Select: Alt+s"));
  _remove->setToolTip(tr("Remove: Alt+r"));


  connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(_up, SIGNAL(clicked()), this, SLOT(upButtonClicked()));
  connect(_down, SIGNAL(clicked()), this, SLOT(downButtonClicked()));

  connect(_add, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_remove, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_up, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_down, SIGNAL(clicked()), this, SIGNAL(modified()));

  connect(_availableRelationList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(availableDoubleClicked(QListWidgetItem*)));
  connect(_displayedRelationList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(displayedDoubleClicked(QListWidgetItem*)));

  connect(_availableRelationList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_displayedRelationList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));

  connect(_editSelectedAvailable, SIGNAL(clicked()), this, SLOT(editSelectedAvailable()));
  connect(_editSelectedDisplayed, SIGNAL(clicked()), this, SLOT(editSelectedDisplayed()));
  connect(_curveSearchEntry, SIGNAL(textChanged(QString)), this, SLOT(filterCurves(QString)));
  connect(_curveSearch, SIGNAL(clicked()), this, SLOT(searchCurves()));

}


ContentTab::~ContentTab() {
}

void ContentTab::editSelectedAvailable() {
  QList<QListWidgetItem *> selected = _availableRelationList->selectedItems();

  if (selected.count()>1) {
    QList<ObjectPtr> objects;
    int n = selected.count();
    for (int i=0; i<n; i++) {
      objects.append(_store->retrieveObject(selected.at(i)->text()));
    }
    DialogLauncher::self()->showMultiObjectDialog(objects);
  } else if (selected.count() > 0) {
    QString name = selected.at(0)->text();
    RelationPtr relation = kst_cast<Relation>(_store->retrieveObject(name));
    DialogLauncher::self()->showObjectDialog(relation);
  }
}

void ContentTab::editSelectedDisplayed() {
  QList<QListWidgetItem *> selected = _displayedRelationList->selectedItems();


  if (selected.count()>1) {
    QList<ObjectPtr> objects;
    int n = selected.count();
    for (int i=0; i<n; i++) {
      objects.append(_store->retrieveObject(selected.at(i)->text()));
    }
    DialogLauncher::self()->showMultiObjectDialog(objects);
  } else if (selected.count() > 0) {
    QString name = selected.at(0)->text();
    RelationPtr relation = kst_cast<Relation>(_store->retrieveObject(name));
    DialogLauncher::self()->showObjectDialog(relation);
  }
}

void ContentTab::updateButtons() {

  QList<QListWidgetItem *> displayedItems = _displayedRelationList->selectedItems();
  QListWidgetItem *displayedItem = 0;

  if (displayedItems.count() > 0)
    displayedItem = displayedItems.first();

  _remove->setEnabled(displayedItems.count() > 0);

  _up->setEnabled(_displayedRelationList->row(displayedItem) > 0);
  _down->setEnabled(_displayedRelationList->row(displayedItem) >= 0 && _displayedRelationList->row(displayedItem) < (int)_displayedRelationList->count() - 1);

  _add->setEnabled(_availableRelationList->selectedItems().count() > 0);
}


void ContentTab::removeButtonClicked() {
  foreach (QListWidgetItem* item, _displayedRelationList->selectedItems()) {
    _availableRelationList->addItem(_displayedRelationList->takeItem(_displayedRelationList->row(item)));
  }

  _availableRelationList->clearSelection();
  updateButtons();
}


void ContentTab::displayedDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _availableRelationList->addItem(_displayedRelationList->takeItem(_displayedRelationList->row(item)));
    _availableRelationList->clearSelection();
    emit modified();
    updateButtons();
  }
}


void ContentTab::addButtonClicked() {
  foreach (QListWidgetItem* item, _availableRelationList->selectedItems()) {
    _displayedRelationList->addItem(_availableRelationList->takeItem(_availableRelationList->row(item)));
  }
  _displayedRelationList->clearSelection();
  updateButtons();
}


void ContentTab::availableDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _displayedRelationList->addItem(_availableRelationList->takeItem(_availableRelationList->row(item)));
    _displayedRelationList->clearSelection();
    emit modified();
    updateButtons();
  }
}


void ContentTab::upButtonClicked() {
  _displayedRelationList->setFocus();

  int i = _displayedRelationList->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _displayedRelationList->takeItem(i);
    _displayedRelationList->insertItem(i-1, item);
    _displayedRelationList->clearSelection();
    _displayedRelationList->setCurrentItem(item);
    //item->setSelected(true);
    updateButtons();
  }
}


void ContentTab::downButtonClicked() {
  _displayedRelationList->setFocus();
  // move item down
  int i = _displayedRelationList->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _displayedRelationList->takeItem(i);
    _displayedRelationList->insertItem(i+1, item);
    _displayedRelationList->clearSelection();
    _displayedRelationList->setCurrentItem(item);
    //item->setSelected(true);
    updateButtons();
  }
}


void ContentTab::setDisplayedRelations(QStringList displayedRelations, QStringList displayedRelationTips) {
  _displayedRelationList->clear();
  _displayedRelationList->addItems(displayedRelations);
  for (int i=0; i<_displayedRelationList->count(); i++) {
    _displayedRelationList->item(i)->setToolTip(displayedRelationTips.at(i));
  }
}


void ContentTab::setAvailableRelations(QStringList availableRelations, QStringList availableRelationTips) {
  _availableRelationList->clear();
  _availableRelationList->addItems(availableRelations);
  for (int i=0; i<_availableRelationList->count(); i++) {
    _availableRelationList->item(i)->setToolTip(availableRelationTips.at(i));
  }
}


QStringList ContentTab::displayedRelations() {
  QStringList relations;
  for (int i = 0; i < _displayedRelationList->count(); i++) {
    relations.append(_displayedRelationList->item(i)->text());
  }
  return relations;
}

void ContentTab::addObject(QString x) {
    for(int i=0;i<_availableRelationList->count();i++) {
        if(_availableRelationList->item(i)->text().contains(x)) {
            _displayedRelationList->addItem(_availableRelationList->takeItem(i));
            return;
        }
    }
}

void ContentTab::removeObject(QString x) {
    for(int i=0;i<_displayedRelationList->count();i++) {
        if(_displayedRelationList->item(i)->text().contains(x)) {
            _availableRelationList->addItem(_displayedRelationList->takeItem(i));
            return;
        }
    }
}

void ContentTab::filterCurves(const QString &filter) {
  _availableRelationList->clearSelection();

  if (filter=="*") { // optimization
    _availableRelationList->selectAll();
    return;
  }

  QRegExp re(filter, Qt::CaseSensitive, QRegExp::Wildcard);
  QStringList selected;

  for (int i = 0; i < _availableRelationList->count(); i++) {
    QListWidgetItem *item = _availableRelationList->item(i);
    if (re.exactMatch(item->text())) {
      item = _availableRelationList->takeItem(i);
      selected.append(item->text());
      i--;
    }
  }

  _availableRelationList->insertItems(0, selected);

  // special case optimization:
  // selecting and unselecting individual items is expensive,
  // but selecting all of them is fast,
  // so either select or select all, then unselect, which ever is fewer.
  if (selected.count() > _availableRelationList->count()/2) {
    _availableRelationList->selectAll();
    for (int i=selected.count(); i<_availableRelationList->count(); i++) {
      _availableRelationList->item(i)->setSelected(false);
    }
  } else {
    for (int i=0; i<selected.count(); i++) {
      _availableRelationList->item(i)->setSelected(true);
    }
  }

  if (selected.count()>0) {
    _availableRelationList->scrollToTop();
  }

}

void ContentTab::searchCurves() {
  QString s = _curveSearchEntry->text();
  if (!s.isEmpty()) {
    if (s[0] != '*') {
      s = '*' + s;
    }
    if (s[s.length()-1] != '*') {
      s += '*';
    }
    _curveSearchEntry->setText(s);
  }
}

}

// vim: ts=2 sw=2 et
