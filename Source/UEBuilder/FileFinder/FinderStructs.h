//
// Created by Stalker7274 on 08.03.2025.
//
#include <QString>

#ifndef UEBUILDER_FINDERSTRUCTS_H
#define UEBUILDER_FINDERSTRUCTS_H

#endif //UEBUILDER_FINDERSTRUCTS_H

struct UProject{
public:

    QString ProjectPath;

    QString ProjectName;

    QString UEditorPath;

    float UEditorVersion;

    UProject(){

        ProjectPath = "";

        ProjectName = "";

        UEditorPath = "";

        UEditorVersion = -1;
    }

    explicit UProject(QString ProjectPath) {

        ProjectPath = ProjectPath;

        QFileInfo fileInfo(ProjectPath);
        ProjectName = fileInfo.fileName();

        UEditorPath = "";

        UEditorVersion = -1;

        QFile projectFile(ProjectPath);
        if (projectFile.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(projectFile.readAll());
            if (!doc.isNull() && doc.isObject()) {
                QJsonObject obj = doc.object();

                if (obj.contains("EngineAssociation")) {
                    QString versionStr = obj["EngineAssociation"].toString();

                    UEditorVersion = versionStr.toFloat();
                }
            }
            projectFile.close();
        }

        QFileInfo projectInfo(ProjectPath);
        QString configPath = projectInfo.dir().absolutePath() + "/Configs/Config.json";
        QFile enginePathsFile(configPath);

        if (enginePathsFile.open(QIODevice::ReadOnly)) {
            QJsonDocument pathsDoc = QJsonDocument::fromJson(enginePathsFile.readAll());
            if (!pathsDoc.isNull() && pathsDoc.isObject()) {
                QJsonObject pathsObj = pathsDoc.object();

                QString versionStr = QString::number(UEditorVersion, 'f', 1);

                for (const QString &key: pathsObj.keys()) {

                    QString pathValue = "U:/Engines/UE_5.4/Engine/Binaries/Win64/UnrealEditor.exe"; //pathsObj[key].toObject()["path"].toString();

                    if (!pathValue.isEmpty() && pathValue.contains(versionStr)) {
                        UEditorPath = pathValue;
                        break;
                    }
                }
            }
            enginePathsFile.close();
        }
    }
};