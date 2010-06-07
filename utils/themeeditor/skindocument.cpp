/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2010 Robert Bieber
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include "skindocument.h"

#include <QFile>
#include <QSettings>
#include <QColor>
#include <QMessageBox>
#include <QFileDialog>

SkinDocument::SkinDocument(QWidget *parent) : QWidget(parent)
{
    setupUI();

    title = "Untitled";
    fileName = "";
    saved = "";
}

SkinDocument::SkinDocument(QString file, QWidget *parent):
        QWidget(parent), fileName(file)
{
    setupUI();

    /* Loading the file */
    if(QFile::exists(fileName))
    {
        QFile fin(fileName);
        fin.open(QFile::ReadOnly);
        editor->document()->setPlainText(QString(fin.readAll()));
        saved = editor->document()->toPlainText();
        fin.close();
    }

    /* Setting the title */
    QStringList decomposed = fileName.split('/');
    title = decomposed.last();
}

SkinDocument::~SkinDocument()
{
    delete highlighter;
    delete model;
}

void SkinDocument::connectPrefs(PreferencesDialog* prefs)
{
    QObject::connect(prefs, SIGNAL(accepted()),
                     this, SLOT(colorsChanged()));
    QObject::connect(prefs, SIGNAL(accepted()),
                     highlighter, SLOT(loadSettings()));
}

bool SkinDocument::requestClose()
{
    if(editor->document()->toPlainText() != saved)
    {
        /* Spawning the "Are you sure?" dialog */
        QMessageBox confirm(this);
        confirm.setWindowTitle(tr("Confirm Close"));
        confirm.setText(title + tr(" has been modified."));
        confirm.setInformativeText(tr("Do you want to save your changes?"));
        confirm.setStandardButtons(QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
        confirm.setDefaultButton(QMessageBox::Save);
        int confirmation = confirm.exec();

        switch(confirmation)
        {
        case QMessageBox::Save:
            save();
            /* After calling save, make sure the user actually went through */
            if(editor->document()->toPlainText() != saved)
                return false;
            else
                return true;

        case QMessageBox::Discard:
            return true;

        case QMessageBox::Cancel:
            return false;
        }
    }

    return true;
}

void SkinDocument::setupUI()
{
    /* Setting up the text edit */
    layout = new QHBoxLayout;
    editor = new QPlainTextEdit(this);
    layout->addWidget(editor);

    setLayout(layout);

    /* Attaching the syntax highlighter */
    highlighter = new SkinHighlighter(editor->document());

    /* Setting up the model */
    model = new ParseTreeModel("");

    /* Connecting the editor's signal */
    QObject::connect(editor, SIGNAL(textChanged()),
                     this, SLOT(codeChanged()));

    colorsChanged();
}

void SkinDocument::colorsChanged()
{
    /* Setting the editor colors */
    QSettings settings;
    settings.beginGroup("SkinDocument");

    QColor fg = settings.value("fgColor", Qt::black).value<QColor>();
    QColor bg = settings.value("bgColor", Qt::white).value<QColor>();
    QPalette palette;
    palette.setColor(QPalette::All, QPalette::Base, bg);
    palette.setColor(QPalette::All, QPalette::Text, fg);

    editor->setPalette(palette);
    editor->repaint();

    settings.endGroup();

}

void SkinDocument::codeChanged()
{
    model->changeTree(editor->document()->toPlainText().toAscii());

    if(editor->document()->toPlainText() != saved)
        emit titleChanged(title + QChar('*'));
    else
        emit titleChanged(title);
}

void SkinDocument::save()
{
    QFile fout(fileName);

    if(!fout.exists())
    {
        saveAs();
        return;
    }

    fout.open(QFile::WriteOnly);
    fout.write(editor->document()->toPlainText().toAscii());
    fout.close();

    saved = editor->document()->toPlainText();
    QStringList decompose = fileName.split('/');
    title = decompose.last();
    emit titleChanged(title);

}

void SkinDocument::saveAs()
{
    /* Determining the directory to open */
    QString directory = fileName;

    QSettings settings;
    settings.beginGroup("SkinDocument");
    if(directory == "")
        directory = settings.value("defaultDirectory", "").toString();

    fileName = QFileDialog::getSaveFileName(this, tr("Save Document"),
                                            directory, fileFilter());
    directory = fileName;
    if(fileName == "")
        return;

    directory.chop(fileName.length() - fileName.lastIndexOf('/') - 1);
    settings.setValue("defaultDirectory", directory);
    settings.endGroup();

    QFile fout(fileName);
    fout.open(QFile::WriteOnly);
    fout.write(editor->document()->toPlainText().toAscii());
    fout.close();

    saved = editor->document()->toPlainText();
    QStringList decompose = fileName.split('/');
    title = decompose[decompose.count() - 1];
    emit titleChanged(title);

}
