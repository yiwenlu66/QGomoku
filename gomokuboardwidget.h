#ifndef GOMOKUBOARDWIDGET_H
#define GOMOKUBOARDWIDGET_H

#include <QWidget>
#include <QSize>
#include <QPaintEvent>
#include <QList>
#include <QDataStream>
#include <QPoint>
#include <QStringMatcher>

class GomokuBoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GomokuBoardWidget(QWidget *parent = 0);

    enum Color {
        BLACK = 1,
        WHITE = 2
    };

    static const int BOARD_SIZE = 15;
    static const QList<int> CRITICAL_INDICES;    // draw thick dots at these lattices

    // serialization / deserialization
    friend QDataStream &operator<<(QDataStream &, const GomokuBoardWidget &);
    friend QDataStream &operator>>(QDataStream &, GomokuBoardWidget &);

    int heightForWidth(int width) const {
        return width;
    }
    QSize sizeHint() const {
        return QSize(100, 100);
    }

    /*
     * Return true for success, false for failure.
     * Emit win(Color) is the color wins.
     */
    bool putPieceAtIndex(Color, int xIndex, int yIndex);
    bool putPieceAtIndex(Color, int index);

    /*
     * Translate xPos, yPos to indices and call putPieceAtIndex() if the position is close enough to a lattice.
     */
    bool putPieceAtPos(Color, int xPos, int yPos);
    bool putPieceAtPos(Color, const QPoint&);

    /*
     * Check for on which lattices the player of the specified color put for the next step
     * can she ensure victory.
     * If any, "bomb" icons will be put on these lattices.
     */
    void checkForChance(Color);

    bool isActive();

public slots:
    void clear();

signals:
    void win(GomokuBoardWidget::Color);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int m_data[BOARD_SIZE * BOARD_SIZE];    // fill in 0 for empty, 3 for bombs
    int atIndex(int xIndex, int yIndex);    // return -1 if out of bound

    bool m_isActive;    // inactivate when one side wins; activate when clearing

    // geometry
    int m_gridSize;
    int m_pieceRadius;
    QPoint m_latticePos[BOARD_SIZE * BOARD_SIZE];
    static int sqtDist(const QPoint &p1, const QPoint &p2);

    static constexpr int const DIRECTIONS[4][2] = { { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 } };
    static const QList<QStringMatcher> PATTERNS;

    /*
     * Check whether the color is winning.
     * The indices indicate where to start the search.
     */
    bool checkWin(Color, int xIndex, int yIndex);
    bool checkWin(Color, int index);

    bool checkForChanceAt(Color color, int index);

    void clearBombs();
};

/* Serialize/deserialize boards for data transmission */
QDataStream &operator<<(QDataStream &, const GomokuBoardWidget &);
QDataStream &operator>>(QDataStream &, GomokuBoardWidget &);

#endif // GOMOKUBOARDWIDGET_H
