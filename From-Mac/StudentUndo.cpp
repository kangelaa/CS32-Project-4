#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
    UndoInfo submission(action, row, col, ch);
    
    if (m_stackOfUndos.size() == 0){    //if nothing in the stack
        m_stackOfUndos.push(submission);    //push the action
    } else {
        UndoInfo past = m_stackOfUndos.top();  //examine the last pushed action to stack
        if (past.act == action && action == Action::DELETE && row == past.row && col == past.col){   //delete batching
            m_stackOfUndos.top().chars += ch;
        } else if (past.act == action && action == Action::DELETE && row == past.row && col == past.col-1){    //backspace batching
            m_stackOfUndos.top().chars = ch + m_stackOfUndos.top().chars;
            m_stackOfUndos.top().col--;
        } else if (past.act == action && action == Action::INSERT && row == past.row && col == past.col+1){    //insert batching
            m_stackOfUndos.top().count++;   //update the count of top item by 1
            m_stackOfUndos.top().col++; //update the col pos to be one to the right
        } else {
            m_stackOfUndos.push(submission);    //push the action
        }
    }
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
    if (m_stackOfUndos.empty()){
        return Action::ERROR;
    } else {
        UndoInfo curUndo = m_stackOfUndos.top();    //save the undo info before popping the top operation off
        m_stackOfUndos.pop();
        if (curUndo.act == Action::INSERT){     //to undo an insertion
            row = curUndo.row;
            col = curUndo.col;
            count = curUndo.count;      //set count equal to number of char to delete
            text = "";
            return Action::DELETE;
        } else if (curUndo.act == Action::DELETE){  //to undo a deletion
            row = curUndo.row;
            col = curUndo.col;  
            count = 1;
            text = curUndo.chars;       //set text equal to string to be inserted
            return Action::INSERT;
        } else if (curUndo.act == Action::SPLIT){   //to undo a split
            row = curUndo.row;
            col = curUndo.col;
            count = 1;
            text = "";
            return Action::JOIN;
        } else {    //if curUndo.act == JOIN        //to undo a join
            row = curUndo.row;
            col = curUndo.col;
            count = 1;
            text = "";
            return Action::SPLIT;
        }
    }
}

void StudentUndo::clear() {
    while (m_stackOfUndos.size() != 0){
        m_stackOfUndos.pop();       //pop off top element of stack until it's empty
    }
}
