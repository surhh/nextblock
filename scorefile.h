#ifndef SCOREFILE_H
#define SCOREFILE_H

#include <QString>
#include <QFile>
#include <QDir>
#include <QtXml>
#include <QDomDocument>
#include <QDebug>
#include <QTextStream>

#include <vector>

#include "engine/gamefieldthread.h"

constexpr uint32_t MAX_TOP_SCORE_ENTRY_COUNT = 10;

struct Player
{
    QString  name;
    GameStat stat;

    bool operator == (const Player &a_player) const
    {
        return (name == a_player.name &&
                stat.blocks == a_player.stat.blocks &&
                stat.delLines == a_player.stat.delLines &&
                stat.score == a_player.stat.score);
    }
};

class ScoreFile
{
private:
    QString     m_fileName;

    std::vector<Player>   m_statData;

public:
    ScoreFile();
    ~ScoreFile();

    void init();
    void setFileName(const QString& a_fileName);
    qint32 loadFile();
    qint32 saveFile();
    void addPlayer(const Player& a_player);
    std::vector<Player> getPlayersData() const;
    quint64 getMinScore() const;
    qint32 findPLayerIndex(const Player& a_player) const;

private:
    void sortStatData();
};

#endif // SCOREFILE_H
