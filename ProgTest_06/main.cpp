#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#endif /* __PROGTEST__ */


class CCell {
public:
      virtual ~CCell()= default;

      virtual int getWidth() const = 0;
      virtual int getHeight() const = 0;

      virtual std::string renderLine(int row, int width, int height) const = 0;

      virtual bool operator==(const CCell& c) const = 0;
      virtual bool operator!=(const CCell& c) const = 0;

      virtual std::shared_ptr<CCell> clone() const  = 0;
};


class CEmpty : public CCell
{
public:
      CEmpty() {}
      int getWidth() const override {
            return 0;
      }
      int getHeight() const override {
            return 0;
      }
      std::shared_ptr<CCell> clone() const override {
            return std::make_shared<CEmpty>();
      }
      std::string renderLine(int row, int width, int height) const override {
            return std::string(width, ' ');
      }
      bool operator==(const CCell& other) const override {
            return dynamic_cast<const CEmpty*>(&other) != nullptr;
      }
      bool operator!=(const CCell& other) const override {
            return !(*this == other);
      }
};


class CText : public CCell
{
public:
    enum Alignment { ALIGN_LEFT, ALIGN_RIGHT };

    CText(const std::string &text, Alignment alignment) : alignment(alignment){
          setText(text);
    }

    void setText(const std::string &text) {
          lines.clear();
          std::istringstream iss(text);
          std::string line;
          width = 0;
          while (std::getline(iss, line)) {
                width = std::max(width, static_cast<int>(line.size()));
                lines.push_back(line);
          }
    }

    int getWidth() const override {
          return width;
    }

    int getHeight() const override {
          return lines.size();
    }

    std::shared_ptr<CCell> clone() const override {
          std::shared_ptr<CText> copy = std::make_shared<CText>("", alignment);
          copy->lines.clear();
          copy->width = width;
          for (auto line : lines) {
                copy->lines.push_back(line);
          }
          return copy;
    }

    std::string renderLine(int row, int width, int height) const override {
          if (row >= (int)lines.size())
                return std::string(width, ' ');
          int padding = width - lines[row].size();

          if (alignment == ALIGN_LEFT)
                return  lines[row] + std::string(padding, ' ');

          return  std::string(padding, ' ') + lines[row];
    }

    bool operator==(const CCell &other) const override {
          const CText * tmp = dynamic_cast<const CText *>(&other);
          if (!tmp) return false;
          if (alignment != tmp->alignment) return false;
          if (getWidth() != tmp->getWidth()) return false;
          if (getHeight() != tmp->getHeight()) return false;
          for (int i = 0; i < lines.size(); i++) {
                if (lines[i] != tmp->lines[i]) return false;
          }
          return true;
    }

    bool operator!=(const CCell &other) const override {
            return !(*this == other);
    }

private:
    std::vector<std::string> lines;
    int width;
    Alignment alignment;
};


class CImage : public CCell
{
public:
    CImage() : _width(0), _height(0) {}

    CImage &addRow(const std::string &row)
    {
          rows.push_back(row);
          ++_height;
          _width = std::max(_width, static_cast<int>(row.length()));
          return *this;
    }

    int getWidth() const override
    {
          return _width;
    }

    int getHeight() const override
    {
          return _height;
    }

      std::string renderLine(int row, int width, int height) const override
    {
          // Calculate vertical padding
          int paddingTop = (height - _height) / 2;
          int paddingBottom = height - _height - paddingTop;

          // Check if the row is within the vertical padding
          if (row < paddingTop || row >= height - paddingBottom)
                return std::string(width, ' ');

          // Adjust row index to account for padding
          int adjustedRow = row - paddingTop;

          // Calculate horizontal padding
          int paddingLeft = (width - rows[adjustedRow].length()) / 2;
          int paddingRight = width - rows[adjustedRow].length() - paddingLeft;

          // Return the centered line
          return std::string(paddingLeft, ' ') + rows[adjustedRow] + std::string(paddingRight, ' ');
    }

    std::shared_ptr<CCell> clone() const override
    {
          std::shared_ptr<CImage> copy = std::make_shared<CImage>();
          copy->_height = getHeight();
          copy->_width = getWidth();
          for (auto row : rows) {
                copy->rows.push_back(row);
          }
          return copy;
    }

    bool operator==(const CCell &other) const override
    {
          const CImage *tmp = dynamic_cast<const CImage*>(&other);
          if (!tmp) return false;
          if (_width != tmp->_width) return false;
          if (_height != tmp->_height) return false;
          for (int i = 0; i < rows.size(); i++) {
                if (rows[i] != tmp->rows[i]) return false;
          }
          return true;
    }

    bool operator!=(const CCell &c) const override
    {
            return !(*this == c);
    }

private:
    std::vector<std::string> rows;
    int _width;
    int _height;
};


class CTable
{
public:
      CTable(int rows, int cols) : cols(cols), rows(rows)
      {
            table.resize(rows, std::vector<std::shared_ptr<CCell>>(cols, std::make_shared<CEmpty>()));
      }

      void setCell(int row, int col, const CCell &cell)
      {
            table[row][col] = cell.clone();
      }

      CCell &getCell(int row, int col)
      {
            return *table[row][col];
      }
      friend std::ostream &operator<<(std::ostream &os, const CTable &table)
      {
            std::vector<int> colWidths(table.cols, 0);

            // Calculate column widths
            for (int col = 0; col < table.cols; ++col)
            {
                  for (int row = 0; row < table.rows; ++row)
                  {
                        colWidths[col] = std::max(colWidths[col], table.table[row][col]->getWidth());
                  }
            }

            // Render the table
            auto renderSeparator = [&]()
            {
                  os << "+";
                  for (int col = 0; col < table.cols; ++col)
                  {
                        os << std::string(colWidths[col], '-') << "+";
                  }
                  os << "\n";
            };

            renderSeparator();
            for (int row = 0; row < table.rows; ++row)
            {
                  int maxHeight = 0;
                  for (int col = 0; col < table.cols; ++col)
                  {
                        maxHeight = std::max(maxHeight, table.table[row][col]->getHeight());
                  }

                  for (int line = 0; line < maxHeight; ++line)
                  {
                        os << "|";
                        for (int col = 0; col < table.cols; ++col)
                        {
                              os << table.table[row][col]->renderLine(line, colWidths[col], maxHeight) << "|";
                        }
                        os << "\n";
                  }
                  renderSeparator();
            }

            return os;
      }



      bool operator==(const CTable &other) const
      {
            if (cols != other.cols) return false;
            if (rows != other.rows) return false;
            for (int i = 0; i < rows; i++) {
                  for (int j = 0; j < cols; j++) {
                        if (table[i][j] != other.table[i][j]) return false;
                  }
            }
            return true;
      }

      bool operator!=(const CTable &other) const
      {
            return !(*this == other);
      }

      int cols;
      int rows;
      std::vector<std::vector<std::shared_ptr<CCell>>> table;
};

#ifndef __PROGTEST__
int main ()
{
  std::ostringstream oss;
  CTable t0 ( 3, 2 );
  t0 . setCell ( 0, 0, CText ( "Hello,\n"
        "Hello Kitty", CText::ALIGN_LEFT ) );
  t0 . setCell ( 1, 0, CText ( "Lorem ipsum dolor sit amet", CText::ALIGN_LEFT ) );
  t0 . setCell ( 2, 0, CText ( "Bye,\n"
        "Hello Kitty", CText::ALIGN_RIGHT ) );
  t0 . setCell ( 1, 1, CImage ()
        . addRow ( "###                   " )
        . addRow ( "#  #                  " )
        . addRow ( "#  # # ##   ###    ###" )
        . addRow ( "###  ##    #   #  #  #" )
        . addRow ( "#    #     #   #  #  #" )
        . addRow ( "#    #     #   #  #  #" )
        . addRow ( "#    #      ###    ###" )
        . addRow ( "                     #" )
        . addRow ( "                   ## " )
        . addRow ( "                      " )
        . addRow ( " #    ###   ###   #   " )
        . addRow ( "###  #   # #     ###  " )
        . addRow ( " #   #####  ###   #   " )
        . addRow ( " #   #         #  #   " )
        . addRow ( "  ##  ###   ###    ## " ) );
  t0 . setCell ( 2, 1, CEmpty () );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+--------------------------+----------------------+\n"
        "|Hello,                    |                      |\n"
        "|Hello Kitty               |                      |\n"
        "+--------------------------+----------------------+\n"
        "|Lorem ipsum dolor sit amet|###                   |\n"
        "|                          |#  #                  |\n"
        "|                          |#  # # ##   ###    ###|\n"
        "|                          |###  ##    #   #  #  #|\n"
        "|                          |#    #     #   #  #  #|\n"
        "|                          |#    #     #   #  #  #|\n"
        "|                          |#    #      ###    ###|\n"
        "|                          |                     #|\n"
        "|                          |                   ## |\n"
        "|                          |                      |\n"
        "|                          | #    ###   ###   #   |\n"
        "|                          |###  #   # #     ###  |\n"
        "|                          | #   #####  ###   #   |\n"
        "|                          | #   #         #  #   |\n"
        "|                          |  ##  ###   ###    ## |\n"
        "+--------------------------+----------------------+\n"
        "|                      Bye,|                      |\n"
        "|               Hello Kitty|                      |\n"
        "+--------------------------+----------------------+\n" );
  t0 . setCell ( 0, 1, t0 . getCell ( 1, 1 ) );
  t0 . setCell ( 2, 1, CImage ()
        . addRow ( "*****   *      *  *      ******* ******  *" )
        . addRow ( "*    *  *      *  *      *            *  *" )
        . addRow ( "*    *  *      *  *      *           *   *" )
        . addRow ( "*    *  *      *  *      *****      *    *" )
        . addRow ( "****    *      *  *      *         *     *" )
        . addRow ( "*  *    *      *  *      *        *       " )
        . addRow ( "*   *   *      *  *      *       *       *" )
        . addRow ( "*    *    *****   ****** ******* ******  *" ) );
  dynamic_cast<CText &> ( t0 . getCell ( 1, 0 ) ) . setText ( "Lorem ipsum dolor sit amet,\n"
        "consectetur adipiscing\n"
        "elit. Curabitur scelerisque\n"
        "lorem vitae lectus cursus,\n"
        "vitae porta ante placerat. Class aptent taciti\n"
        "sociosqu ad litora\n"
        "torquent per\n"
        "conubia nostra,\n"
        "per inceptos himenaeos.\n"
        "\n"
        "Donec tincidunt augue\n"
        "sit amet metus\n"
        "pretium volutpat.\n"
        "Donec faucibus,\n"
        "ante sit amet\n"
        "luctus posuere,\n"
        "mauris tellus" );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  CTable t1 ( t0 );
  t1 . setCell ( 1, 0, CEmpty () );
  t1 . setCell ( 1, 1, CEmpty () );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+-----------+------------------------------------------+\n"
        "|Hello,     |          ###                             |\n"
        "|Hello Kitty|          #  #                            |\n"
        "|           |          #  # # ##   ###    ###          |\n"
        "|           |          ###  ##    #   #  #  #          |\n"
        "|           |          #    #     #   #  #  #          |\n"
        "|           |          #    #     #   #  #  #          |\n"
        "|           |          #    #      ###    ###          |\n"
        "|           |                               #          |\n"
        "|           |                             ##           |\n"
        "|           |                                          |\n"
        "|           |           #    ###   ###   #             |\n"
        "|           |          ###  #   # #     ###            |\n"
        "|           |           #   #####  ###   #             |\n"
        "|           |           #   #         #  #             |\n"
        "|           |            ##  ###   ###    ##           |\n"
        "+-----------+------------------------------------------+\n"
        "+-----------+------------------------------------------+\n"
        "|       Bye,|*****   *      *  *      ******* ******  *|\n"
        "|Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|           |*    *  *      *  *      *           *   *|\n"
        "|           |*    *  *      *  *      *****      *    *|\n"
        "|           |****    *      *  *      *         *     *|\n"
        "|           |*  *    *      *  *      *        *       |\n"
        "|           |*   *   *      *  *      *       *       *|\n"
        "|           |*    *    *****   ****** ******* ******  *|\n"
        "+-----------+------------------------------------------+\n" );
  t1 = t0;
  t1 . setCell ( 0, 0, CEmpty () );
  t1 . setCell ( 1, 1, CImage ()
        . addRow ( "  ********                    " )
        . addRow ( " **********                   " )
        . addRow ( "**        **                  " )
        . addRow ( "**             **        **   " )
        . addRow ( "**             **        **   " )
        . addRow ( "***         ********  ********" )
        . addRow ( "****        ********  ********" )
        . addRow ( "****           **        **   " )
        . addRow ( "****           **        **   " )
        . addRow ( "****      **                  " )
        . addRow ( " **********                   " )
        . addRow ( "  ********                    " ) );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                              |          ###                             |\n"
        "|                                              |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |                                          |\n"
        "|elit. Curabitur scelerisque                   |        ********                          |\n"
        "|lorem vitae lectus cursus,                    |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
        "|sociosqu ad litora                            |      **             **        **         |\n"
        "|torquent per                                  |      **             **        **         |\n"
        "|conubia nostra,                               |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
        "|                                              |      ****           **        **         |\n"
        "|Donec tincidunt augue                         |      ****           **        **         |\n"
        "|sit amet metus                                |      ****      **                        |\n"
        "|pretium volutpat.                             |       **********                         |\n"
        "|Donec faucibus,                               |        ********                          |\n"
        "|ante sit amet                                 |                                          |\n"
        "|luctus posuere,                               |                                          |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  assert ( t0 != t1 );
  assert ( !( t0 == t1 ) );
  assert ( t0 . getCell ( 1, 1 ) == t0 . getCell ( 0, 1 ) );
  assert ( ! ( t0 . getCell ( 1, 1 ) != t0 . getCell ( 0, 1 ) ) );
  assert ( t0 . getCell ( 0, 0 ) != t0 . getCell ( 0, 1 ) );
  assert ( ! ( t0 . getCell ( 0, 0 ) == t0 . getCell ( 0, 1 ) ) );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
