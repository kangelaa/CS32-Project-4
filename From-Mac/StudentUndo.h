#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include "stack"

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
    struct UndoInfo {
        UndoInfo(Action act, int row, int col, char ch)
        : act(act), row(row), col(col), ch(ch), chars(""), count(1){
            if (act == Action::DELETE){
                chars += ch;
            }
        }
        Action act;
        int row;
        int col;
        char ch;
        std::string chars;
        int count;
    };
    std::stack<UndoInfo> m_stackOfUndos;
};

#endif // STUDENTUNDO_H_
