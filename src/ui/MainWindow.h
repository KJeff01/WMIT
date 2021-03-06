/*
	Copyright 2010 Warzone 2100 Project

	This file is part of WMIT.

	WMIT is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	WMIT is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with WMIT.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <QList>
#include <QSet>
#include <QPair>

#include <QSettings>
#include <QSignalMapper>
#include <QActionGroup>
#include <QFileSystemWatcher>
#include <QBasicTimer>

#include "QWZM.h"
#include "Pie.h"
#include "wmit.h"

class MaterialDock;
class TransformDock;
class MeshDock;
class ImportDialog;
class ExportDialog;
class TextureDialog;
class UVEditor;

namespace Ui
{
	class MainWindow;
}

struct ModelInfo
{
	ModelInfo() {clear();}

	PieCaps m_pieCaps;
	wmit_filetype_t m_save_type;
	wmit_filetype_t m_read_type;
	QString m_currentFile;
	QString m_saveAsFile;

	void clear()
	{
		m_save_type = m_read_type = WMIT_FT_WZM;
		m_pieCaps.reset();
		m_currentFile.clear();
		m_saveAsFile.clear();
	}

	void defaultPieCapsIfNeeded()
	{
		if (m_read_type != WMIT_FT_PIE && m_read_type != WMIT_FT_PIE2)
			m_pieCaps = m_save_type == WMIT_FT_PIE? PIE3_CAPS : PIE2_CAPS;
	}

	void prepareForSaveToSelf()
	{
		// Use orignal type and filename if we never went through a save before
		if (!m_saveAsFile.isEmpty())
			return;
		m_save_type = m_read_type;
		m_saveAsFile = m_currentFile;
	}
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWZM &model, QWidget *parent = nullptr);
	~MainWindow();

	void clear();
	bool openFile(const QString& file);

	static bool loadModel(const QString& file, WZM& model, ModelInfo &info, bool nogui = false);
	static bool guessModelTypeFromFilename(const QString &fname, wmit_filetype_t &type);
	static bool saveModel(const WZM& model, const ModelInfo &info);

	void PrependFileToRecentList(const QString &filename);

protected:
	void changeEvent(QEvent *event);
	void closeEvent(QCloseEvent *event);
private slots:
	void actionOpen();
	void actionOpenRecent(QAction *action);
	void actionClearRecentFiles();
	void actionClearMissingFiles();
	void actionSave();
	void actionSaveAs();
	void actionClose();
	void actionSetupTextures();
	void actionAppendModel();
	void actionTakeScreenshot();
	void actionSetTeamColor();
	void actionLocateUserShaders();
	void actionReloadUserShader();
	void actionEnableUserShaders(bool checked);
	void actionImport_Animation();
	void actionImport_Connectors();

	void updateRecentFilesMenu();
	void updateModelRender();
	void updateConnectorsView();
	void viewerInitialized();
	void shaderAction(int);

	// transformations
	void scaleXYZChanged(double scale);
	void scaleXChanged(double scale);
	void scaleYChanged(double scale);
	void scaleZChanged(double scale);
	void reverseWindings();
	void flipNormals();
	void mirrorAxis(int axis);
	void removeMesh();
	void centerMesh(int axis);

	void materialChangedFromUI(const WZMaterial& mat);

private:
	Ui::MainWindow *m_ui;

	MaterialDock *m_materialDock;
	TransformDock *m_transformDock;
	MeshDock *m_meshDock;

	TextureDialog *m_textureDialog;
	UVEditor *m_UVEditor;
	QSettings *m_settings;

	QSignalMapper *m_shaderSignalMapper;
	QActionGroup* m_shaderGroup;
	QAction *m_actionEnableUserShaders;
	QAction *m_actionLocateUserShaders;
	QAction *m_actionReloadUserShaders;
	QAction *m_actionEnableTangentInShaders;
	QString m_pathImport, m_pathExport;

	QWZM *m_model;
	ModelInfo m_modelinfo;
	QString m_pathvert, m_pathfrag;

	QString buildAppTitle();
	bool fireTextureDialog(const bool reinit = false);
	bool reloadShader(wz_shader_type_t type, bool user_shader, QString* errMessage = nullptr);
	void doAfterModelWasLoaded(const bool success = true);

	wz_shader_type_t getShaderType() const
	{
		int actChecked = m_shaderGroup->actions().indexOf(m_shaderGroup->checkedAction());
		if (actChecked < 0)
			return WZ_SHADER_NONE;
		return static_cast<wz_shader_type_t>(actChecked);
	}
};

#endif // MAINWINDOW_HPP
