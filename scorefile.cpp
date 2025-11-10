#include "scorefile.h"

ScoreFile::ScoreFile()
{

}

ScoreFile::~ScoreFile()
{

}

void ScoreFile::init()
{
    m_fileName.clear();

    m_statData.clear();
}

std::vector<Player> ScoreFile::getPlayersData() const
{
    return m_statData;
}

void ScoreFile::setFileName(const QString& a_fileName)
{
    m_fileName = QDir::homePath() + "/" + a_fileName;
}

void ScoreFile::addPlayer(const Player& a_player)
{
    m_statData.push_back(a_player);

    sortStatData();
}

quint64 ScoreFile::getMinScore() const
{
    /// as the vector is always sorted by descending order, it's enough to take the value of last element

    quint64 res = 0;

    size_t size = m_statData.size();

    if (size > 0)
    {
        res = m_statData[size - 1].stat.score;
    }

    return res;
}

qint32 ScoreFile::findPLayerIndex(const Player& a_player) const
{
    qint32 index = 1, res = -1;

    for (std::vector<Player>::const_iterator it = m_statData.begin(); it < m_statData.end(); ++it, ++index)
    {
        if ((*it) == a_player)
        {
            res = index;

            break;
        }
    }

    return res;
}


qint32 ScoreFile::loadFile()
{
    qint32 res = 1;

    Player player;

    QFile file(m_fileName);

    m_statData.clear();

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file:" << m_fileName;
        return 0;
    }

    QDomDocument document;

    if (!document.setContent(&file))
    {
        qDebug() << "Failed to parse XML data";
        file.close();
        return 0;
    }

    file.close();


    /// reading root element
    QDomElement root = document.documentElement();

    /// Get all the rest of player elements
    QDomNodeList players = root.elementsByTagName("player");

    for (int i = 0; i < players.count(); ++i)
    {
        QDomElement playerE = players.at(i).toElement();

        if (!playerE.isNull())
        {
            bool b;

            /// Reading the IDs of the player
            QString id = playerE.attribute("id");

            /// Reading the players' saved score and other parameters
            QDomElement nameE = playerE.firstChildElement("name");
            QString name = nameE.text();
            player.name = name;

            QDomElement scoreE = playerE.firstChildElement("score");
            QString score = scoreE.text();
            player.stat.score = score.toLong(&b, 10);
            if (!b)
            {
                player.stat.score = 0;
            }

            QDomElement blocksE = playerE.firstChildElement("blocks");
            QString blocks = blocksE.text();
            player.stat.blocks = blocks.toInt(&b, 10);
            if (!b)
            {
                player.stat.blocks = 0;
            }

            QDomElement delLinesE = playerE.firstChildElement("delLines");
            QString delLines = delLinesE.text();
            player.stat.delLines = delLines.toInt(&b, 10);
            if (!b)
            {
                player.stat.delLines = 0;
            }

            QDomElement effE = playerE.firstChildElement("eff");
            QString eff = effE.text();
            player.stat.eff = eff.toFloat(&b);
            if (!b)
            {
                player.stat.eff = 0.0;
            }

            QDomElement scoreblockE = playerE.firstChildElement("scoreblock");
            QString scoreblock = scoreblockE.text();
            player.stat.scoreblock = scoreblock.toFloat(&b);
            if (!b)
            {
                player.stat.scoreblock = 0;
            }

            addPlayer(player);
        }
    }

    return res;
}

qint32 ScoreFile::saveFile()
{
    qint32 res = 1;

    /// Creating document and root element
    QDomDocument document;

    QDomElement root = document.createElement("root");
    document.appendChild(root);

    quint32 id = 1;

    sortStatData();

    for (std::vector<Player>::const_iterator it = m_statData.begin(); (it < m_statData.end() && id <= MAX_TOP_SCORE_ENTRY_COUNT); ++it, ++id)
    {
        /// Creating a child element
        QDomElement player = document.createElement("player");
        player.setAttribute("id", QString::number(id));

        QDomElement name = document.createElement("name");
        name.appendChild(document.createTextNode((*it).name));
        player.appendChild(name);

        QDomElement score = document.createElement("score");
        score.appendChild(document.createTextNode(QString::number(it->stat.score)));
        player.appendChild(score);

        QDomElement blocks = document.createElement("blocks");
        blocks.appendChild(document.createTextNode(QString::number(it->stat.blocks)));
        player.appendChild(blocks);

        QDomElement delLines = document.createElement("delLines");
        delLines.appendChild(document.createTextNode(QString::number(it->stat.delLines)));
        player.appendChild(delLines);

        QDomElement eff = document.createElement("eff");
        eff.appendChild(document.createTextNode(QString::number(it->stat.eff, 'f', 2)));
        player.appendChild(eff);

        QDomElement scoreblock = document.createElement("scoreblock");
        scoreblock.appendChild(document.createTextNode(QString::number(it->stat.scoreblock, 'f', 2)));
        player.appendChild(scoreblock);

        root.appendChild(player);
    }

    /// Writing the XML file to disk
    QFile file(m_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for writing:" << m_fileName;
        return 0;
    }

    QTextStream stream(&file);
    stream << document.toString(); /// Converting the document to string
    file.close();

    return res;
}

void ScoreFile::sortStatData()
{
    size_t size = m_statData.size();

    for (uint32_t i = 0; i < size; ++i)
    {
        for (uint32_t j = 0; j < size - 1; ++j)
        {
            if (m_statData[j].stat.score < m_statData[j + 1].stat.score)
            {
                Player p = m_statData[j];
                m_statData[j] = m_statData[j + 1];
                m_statData[j + 1] = p;
            }
        }
    }
}
