#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {   //initialize data members
     m_row = 0;
     m_col = 0;
     m_listOfTextLines.push_back("");
     m_iteratorToCurrentRow = m_listOfTextLines.begin();
     m_submitUndo = true;
}

StudentTextEditor::~StudentTextEditor()
{
    m_listOfTextLines.clear();
}

bool StudentTextEditor::load(std::string file) {
    ifstream infile(file);  //create a new ifstream object infile
    if (!infile){   //if failed to create file, return false
        return false;
    }
    reset(); //if current text editor isn't empty, reset contents of text editor
    
    string s;
    while (getline(infile, s)){ //while still not reached end of file
        if (s != "" && s[s.size()-1] == '\r'){     //check for carriage return char at end, remove from string if present
            s.pop_back();
        }
        m_listOfTextLines.push_back(s);     //push string onto list
    }
    if (m_listOfTextLines.size() == 0){ //if loaded in an empty file w/ no lines, push an empty line to list
        m_listOfTextLines.push_back("");
    }
    m_iteratorToCurrentRow = m_listOfTextLines.begin();    //set iterator to equal first line of text
    return true;
}

bool StudentTextEditor::save(std::string file) {
    ofstream outfile(file); //create a new ofstream object outfile
    if (!outfile){  //if failed to create file, return false
        return false;
    }
    for (list<string>::iterator p = m_listOfTextLines.begin(); p!=m_listOfTextLines.end(); p++){    //iterate through list and update the outfile object
        outfile << (*p) << endl;
    }
    return true;
}

void StudentTextEditor::reset() {
    m_listOfTextLines.clear();
    m_row = 0;
    m_col = 0;
    getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
    switch (dir){
        case Dir::UP:
            if (m_row > 0){     //if pos isn't on top line, move pos up 1 row
                m_row--;
                m_iteratorToCurrentRow--;
                if (m_col > (*m_iteratorToCurrentRow).size()){
                    m_col = (*m_iteratorToCurrentRow).size();
                }
            }
            break;
        case Dir::DOWN:
            if (m_row < m_listOfTextLines.size()-1){    //if pos isn't on bottom line, move pos down 1 row
                m_row++;
                m_iteratorToCurrentRow++;
                if (m_col > (*m_iteratorToCurrentRow).size()){
                    m_col = (*m_iteratorToCurrentRow).size();
                }
            }
            break;
        case Dir::LEFT:
            if (m_col > 0){ //if pos isn't on first char, move pos left by 1 char
                m_col--;
            } else if (m_col == 0 && m_row > 0){    //if pos is on first char and on any row BUT the first, move cursor to after the last char on the prev line
                m_row--;
                m_iteratorToCurrentRow--;
                m_col = (*m_iteratorToCurrentRow).size();
            }
            break;
        case Dir::RIGHT:
            if (m_col < (*m_iteratorToCurrentRow).size()){  //if pos isn't 1 after the last char, move pos right by 1 char
                m_col++;
            } else if (m_col == (*m_iteratorToCurrentRow).size() && m_row < m_listOfTextLines.size()-1){    //if pos is 1 after the last char and on any row BUT the last, move cursor to first char of the next line
                m_row++;
                m_iteratorToCurrentRow++;
                m_col = 0;
            }
            break;
        case Dir::HOME:
            m_col = 0;  //moves editing pos to first char of current line
            break;
        case Dir::END:
            m_col = (*m_iteratorToCurrentRow).size();   //moves editing pos to just after the last char of current line
            break;
    }
}

void StudentTextEditor::del() {
    if (m_col < (*m_iteratorToCurrentRow).size()){  //if editing pos on a valid char, delete that char and maintain pos
        char ch = (*m_iteratorToCurrentRow)[m_col]; //save char about to be deleted
        (*m_iteratorToCurrentRow).erase(m_col, 1);
        if (m_submitUndo){  //submit undo info if variable is true w/ saved char
            getUndo()->submit(Undo::Action::DELETE, m_row, m_col, ch);
        }
    } else if (m_row < m_listOfTextLines.size()-1){ //merge lines
        list<string>::iterator iterToNextLine = m_iteratorToCurrentRow;
        iterToNextLine++;
        string tempStr = (*iterToNextLine);   //assign contents of next line to temporary string
        m_listOfTextLines.erase(iterToNextLine);    //erase the next line in the list
        (*m_iteratorToCurrentRow) += tempStr;   //append the contents of the deleted line to the current line/row
        if (m_submitUndo){
            getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
        }
    }
}

void StudentTextEditor::backspace() {
    if (m_col > 0){ //if current editing pos col > 0
        char ch = (*m_iteratorToCurrentRow)[m_col-1];  //save char about to be deleted
        (*m_iteratorToCurrentRow).erase(m_col-1, 1);
        m_col--;
        if (m_submitUndo){ //submit undo info if variable is true w/ saved char
            getUndo()->submit(Undo::Action::DELETE, m_row, m_col, ch);
        }
    } else if (m_iteratorToCurrentRow != m_listOfTextLines.begin()){ //if editing pos is on first char of any line BUT the first
        string tempStr = *m_iteratorToCurrentRow; //assign contents of current line to temporary string
        m_iteratorToCurrentRow = m_listOfTextLines.erase(m_iteratorToCurrentRow);    //erase the current line in the list
        m_iteratorToCurrentRow--;
        m_row--;    //update m_row and m_col accordingly
        m_col = (*m_iteratorToCurrentRow).size();
        (*m_iteratorToCurrentRow) += tempStr;   //append the contents of the deleted line to the current line/row
        if (m_submitUndo){
            getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
        }
    }
}

void StudentTextEditor::insert(char ch) {
    if (ch == '\t'){    //if user inserts tab char, insert 4 spaces + move current editing pos by 4 spots
        (*m_iteratorToCurrentRow).insert(m_col, 4, ' ');
        if (m_submitUndo){
            for (int i=1; i<=4; i++){
                getUndo()->submit(Undo::Action::INSERT, m_row, m_col+i, ' ');
            }
        }
        m_col += 4;
    } else {
        (*m_iteratorToCurrentRow).insert(m_col, 1, ch);   //use insert() to insert ch
        m_col++;    //move current editing pos right by 1 spot
        if (m_submitUndo){
            getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
        }
    }
}

void StudentTextEditor::enter() {
    string tempStr = "";
    for (int i=m_col; i<(*m_iteratorToCurrentRow).size(); i++){  //iterate thru rest of string
        tempStr += (*m_iteratorToCurrentRow)[i];   //add rest of string to tempStr
        (*m_iteratorToCurrentRow).erase(i, 1);
        i--;   //decrement index before u increment for next iteration
    }
    m_iteratorToCurrentRow++;
    m_iteratorToCurrentRow = m_listOfTextLines.insert(m_iteratorToCurrentRow, tempStr); //add tempStr to list in correct place
    if (m_submitUndo){
        getUndo()->submit(Undo::Action::SPLIT, m_row, m_col);
    }
    m_row++;    //update row and col position
    m_col = 0;
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = m_row;
    col = m_col;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
    if (startRow < 0 || numRows < 0 || startRow > m_listOfTextLines.size()){
        return -1;
    }
    lines.clear();
    if (startRow == m_listOfTextLines.size()){
        return 0;
    } else {
        std::list<std::string>::const_iterator tempRowIter = m_iteratorToCurrentRow;    //make copy of current Iter as a const Iter
        int changeRows = startRow - m_row;  //figure out the # of rows you need to move to get to startRow from current row
        for (int i=0; i<abs(changeRows); i++){   //move to the startRow
            if (changeRows < 0){
                tempRowIter--;
            } else {
                tempRowIter++;
            }
        }
        for (int i=0; tempRowIter != m_listOfTextLines.end() && i<numRows; tempRowIter++, i++){
            lines.push_back(*tempRowIter);
        }
        return lines.size();
    }
}

void StudentTextEditor::undo() {
    int row, col, count;
    string text;
    Undo::Action action = getUndo()->get(row, col, count, text);
    
    if (action == Undo::Action::ERROR){
        return;     //return immediately if undo stack is empty
    }
    
    int changeRows = row - m_row;  //figure out the # of rows you need to move to get to row from current row
    for (int i=0; i<abs(changeRows); i++){   //move to the specified row
        if (changeRows < 0){
            m_iteratorToCurrentRow--;       //move iterator up
        } else {
            m_iteratorToCurrentRow++;       //move iterator down
        }
    }
    
    m_row = row;        //update m_row and m_col
    m_col = col;
    
    m_submitUndo = false; //don't submit the undo information
    switch(action){
        case Undo::Action::INSERT:
            for (int i=0; i<text.size(); i++){  //insert string char by char
                insert(text[i]);
            }
            m_col = col;    //set m_col back to col position returned by undo get() command
            break;
        case Undo::Action::DELETE:
            for (int i=0; i<count; i++){    //delete specified count of char
                backspace();
            }
            break;
        case Undo::Action::SPLIT:
            enter();    //add a line break
            m_col = col;
            m_row = row;
            m_iteratorToCurrentRow--;
            break;
        case Undo::Action::JOIN:
            del();  //join 2 lines together (merge)
            break;
    }
    m_submitUndo = true;    //submit undo information from now on
}
