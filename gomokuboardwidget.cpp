#include "gomokuboardwidget.h"

#include <QtDebug>

#include <cstring>

#include <QSizePolicy>
#include <QPainter>
#include <QPalette>

const int GomokuBoardWidget::BOARD_SIZE;
const QList<int> GomokuBoardWidget::CRITICAL_INDICES = { 3, 7, 11 };

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
    if (m_data[index]) {
        return false;
    }
    m_data[index] = static_cast<int>(color);
    if (checkWin(color, index)) {
        emit win(color);
    }
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
            update();
            return true;
        }
    }
    return false;
}

void GomokuBoardWidget::checkForChance(Color color)
{
    // TODO
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
                // TODO: draw bombs
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

bool GomokuBoardWidget::checkWin(Color color, int xIndex, int yIndex)
{
    int streak;

    // search horizontally
    streak = 0;
    for (int x = xIndex; atIndex(x, yIndex) == color; ++x, ++streak)
        ;
    for (int x = xIndex - 1; atIndex(x, yIndex) == color; --x, ++streak)
        ;
    if (streak >= 5) {
        return true;
    }

    // search vertically
    streak = 0;
    for (int y = yIndex; atIndex(xIndex, y) == color; ++y, ++streak)
        ;
    for (int y = yIndex - 1; atIndex(xIndex, y) == color; --y, ++streak)
        ;
    if (streak >= 5) {
        return true;
    }

    // search in pi/4 direction
    streak = 0;
    for (int x = xIndex, y = yIndex; atIndex(x, y) == color; ++x, ++y, ++streak)
        ;
    for (int x = xIndex - 1, y = yIndex - 1; atIndex(x, y) == color; --x, --y, ++streak)
        ;
    if (streak >= 5) {
        return true;
    }

    // search in 3*pi/4 direction
    streak = 0;
    for (int x = xIndex, y = yIndex; atIndex(x, y) == color; --x, ++y, ++streak)
        ;
    for (int x = xIndex + 1, y = yIndex - 1; atIndex(x, y) == color; ++x, --y, ++streak)
        ;
    if (streak >= 5) {
        return true;
    }

    return false;
}

int GomokuBoardWidget::atIndex(int xIndex, int yIndex)
{
    if (xIndex < 0 || xIndex >= BOARD_SIZE || yIndex < 0 || yIndex >= BOARD_SIZE) {
        return -1;
    }
    return m_data[BOARD_SIZE * yIndex + xIndex];
}
