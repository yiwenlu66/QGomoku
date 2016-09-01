#include "gomokuboardwidget.h"

#include <QtDebug>

#include <cstring>

#include <QSizePolicy>
#include <QPainter>
#include <QPalette>
#include <QQueue>
#include <QList>

const int GomokuBoardWidget::BOARD_SIZE;
const QList<int> GomokuBoardWidget::CRITICAL_INDICES = { 3, 7, 11 };
constexpr int const GomokuBoardWidget::DIRECTIONS[4][2];
const QList<QStringMatcher> GomokuBoardWidget::PATTERNS = {
    QStringMatcher("01110"),
    QStringMatcher("010110"),
    QStringMatcher("011010"),
    QStringMatcher("01111"),
    QStringMatcher("11110")
};

GomokuBoardWidget::GomokuBoardWidget(QWidget *parent) : QWidget(parent)
{
    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(215, 173, 143));
    setAutoFillBackground(true);
    setPalette(pal);

    clear();
}

bool GomokuBoardWidget::putPieceAtIndex(Color color, int xIndex, int yIndex)
{
    if (xIndex < 0 || xIndex >= BOARD_SIZE || yIndex < 0 || yIndex >= BOARD_SIZE) {
        return false;
    }
    return putPieceAtIndex(color, BOARD_SIZE * yIndex + xIndex);
}

bool GomokuBoardWidget::putPieceAtIndex(Color color, int index)
{
    if (index < 0 || index >= BOARD_SIZE * BOARD_SIZE) {
        return false;
    }
    if (m_data[index] && m_data[index] != 3) {
        return false;
    }
    clearBombs();
    m_data[index] = static_cast<int>(color);
    if (checkWin(color, index)) {
        emit win(color);
    }
    if (checkTie()) {
        emit tie();
    }
    update();
    return true;
}

bool GomokuBoardWidget::putPieceAtPos(Color color, int xPos, int yPos)
{
    return putPieceAtPos(color, QPoint(xPos, yPos));
}

bool GomokuBoardWidget::putPieceAtPos(Color color, const QPoint &pos)
{
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        if (sqtDist(m_latticePos[i], pos) < m_pieceRadius * m_pieceRadius) {
            putPieceAtIndex(color, i);
            return true;
        }
    }
    return false;
}

void GomokuBoardWidget::checkForChance(Color color)
{
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {

        if (m_data[i]) {
            // ignore non-empty lattices
            continue;
        }

        if (checkForChanceAt(color, i)) {
            m_data[i] = 3;
        }

    }
    update();
}

bool GomokuBoardWidget::checkForChanceAt(Color color, int startIndex)
{
    int cnt = 0;
    QQueue<int> bfsQueue;
    QList<int> searched;
    bfsQueue.enqueue(startIndex);
    while (!bfsQueue.isEmpty()) {
        int index = bfsQueue.dequeue();
        int x = index % BOARD_SIZE;
        int y = index / BOARD_SIZE;
        for (int i = 0; i < 4; ++i) {
            bool isDirectionValid = false;
            QString str;
            for (int j = -4; j <= 4; ++j) {
                if (j == 0) {
                    str.append("1");
                    continue;
                }
                int newX = x + j * DIRECTIONS[i][0];
                int newY = y + j * DIRECTIONS[i][1];
                if (searched.contains(BOARD_SIZE * newY + newX)) {
                    str.append("2");
                } else {
                    int valueAtIndex = atIndex(newX, newY);
                    if (valueAtIndex == 0 || valueAtIndex == 3) {
                        str.append("0");
                    } else if (valueAtIndex == static_cast<int>(color)) {
                        str.append("1");
                    } else {
                        str.append("2");
                    }
                }
            }

            for (auto pattern : PATTERNS) {
                int matchResult = pattern.indexIn(str);
                if (matchResult != -1) {
                    isDirectionValid = true;
                    for (int j = matchResult - 4; j < matchResult + pattern.pattern().length() - 4; ++j) {
                        int newX = x + j * DIRECTIONS[i][0];
                        int newY = y + j * DIRECTIONS[i][1];
                        int newIndex = BOARD_SIZE * newY + newX;
                        if (!searched.contains(newIndex) && !bfsQueue.contains(newIndex) && m_data[newIndex] == static_cast<int>(color)) {
                            bfsQueue.enqueue(newIndex);
                        }
                    }
                }
            }

            if (isDirectionValid) {
                ++cnt;
            }
        }
        searched << index;
    }

    if (cnt >= 2) {
        return true;
    }

    return false;
}

void GomokuBoardWidget::clearBombs()
{
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        if (m_data[i] == 3) {
            m_data[i] = 0;
        }
    }
}

void GomokuBoardWidget::clear()
{
    std::memset(m_data, 0, sizeof m_data);
    m_isActive = true;
}

void GomokuBoardWidget::paintEvent(QPaintEvent *)
{
    // update geometry
    m_gridSize = width() / (BOARD_SIZE + 1);
    m_pieceRadius = m_gridSize * 2 / 5;
    int margin = (width() - (BOARD_SIZE - 1) * m_gridSize) / 2;
    int x = margin, y = margin;
    for (int i = 0; i < BOARD_SIZE; ++i, y += m_gridSize) {
        for (int j = 0; j < BOARD_SIZE; ++j, x += m_gridSize) {
            m_latticePos[i * BOARD_SIZE + j] = QPoint(x, y);
        }
        x = margin;
    }
    int thickDotRadius = m_pieceRadius / 5;
    if (!thickDotRadius) {
        thickDotRadius = 1;
    }

    QPainter painter(this);

    // draw grids
    painter.setPen(Qt::black);
    for (int i = 0; i < BOARD_SIZE; ++i) {
        painter.drawLine(m_latticePos[i], m_latticePos[BOARD_SIZE * (BOARD_SIZE - 1) + i]);
        painter.drawLine(m_latticePos[i * BOARD_SIZE], m_latticePos[(i + 1) * BOARD_SIZE - 1]);
    }

    // draw pieces / thick dots
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            int index = i * BOARD_SIZE + j;
            switch (m_data[index]) {
            case BLACK:
            case WHITE: {
                Qt::GlobalColor color = (m_data[index] == BLACK) ? Qt::black : Qt::white;
                painter.setPen(color);
                painter.setBrush(color);
                painter.drawEllipse(m_latticePos[index], m_pieceRadius, m_pieceRadius);
                break;
            }
            case 3:
                painter.save();
                painter.translate(m_latticePos[index]);
                painter.drawPixmap(-m_pieceRadius, -m_pieceRadius, 2 * m_pieceRadius, 2 * m_pieceRadius, QPixmap(":/icons/bomb.png"));
                painter.restore();
                break;
            case 0:
                if (CRITICAL_INDICES.contains(i) && (i == j || i + j == BOARD_SIZE - 1)) {
                    painter.setPen(Qt::black);
                    painter.setBrush(Qt::black);
                    painter.drawEllipse(m_latticePos[index], thickDotRadius, thickDotRadius);
                }
                break;
            }
        }
    }
}

int GomokuBoardWidget::sqtDist(const QPoint &p1, const QPoint &p2)
{
    int dx = p1.x() - p2.x();
    int dy = p1.y() - p2.y();
    return dx * dx + dy * dy;
}

bool GomokuBoardWidget::checkWin(Color color, int index)
{
    int xIndex = index % BOARD_SIZE;
    int yIndex = index / BOARD_SIZE;
    return checkWin(color, xIndex, yIndex);
}

bool GomokuBoardWidget::checkWin(Color color, int x, int y)
{
    int streak;

    for (int i = 0; i < 4; ++i) {
        streak = 0;
        for (int j = 0; atIndex(x + j * DIRECTIONS[i][0], y + j * DIRECTIONS[i][1]) == color; ++j, ++streak)
            ;
        for (int j = -1; atIndex(x + j * DIRECTIONS[i][0], y + j * DIRECTIONS[i][1]) == color; --j, ++streak)
            ;
        if (streak >= 5) {
            return true;
        }
    }

    return false;
}

bool GomokuBoardWidget::checkTie()
{
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        if (m_data[i] == 0 || m_data[i] == 3) {
            return false;
        }
    }
    return true;
}

int GomokuBoardWidget::atIndex(int xIndex, int yIndex)
{
    if (xIndex < 0 || xIndex >= BOARD_SIZE || yIndex < 0 || yIndex >= BOARD_SIZE) {
        return -1;
    }
    return m_data[BOARD_SIZE * yIndex + xIndex];
}

QDataStream &operator<<(QDataStream &out, const GomokuBoardWidget &board)
{
    for (int i = 0; i < GomokuBoardWidget::BOARD_SIZE * GomokuBoardWidget::BOARD_SIZE; ++i) {
        out << board.m_data[i];
    }
    return out;
}

QDataStream &operator>>(QDataStream &in, GomokuBoardWidget &board)
{
    for (int i = 0; i < GomokuBoardWidget::BOARD_SIZE * GomokuBoardWidget::BOARD_SIZE; ++i) {
        in >> board.m_data[i];
    }
    return in;
}
