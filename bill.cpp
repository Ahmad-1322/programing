/*NAME:ALHAJ MOHAMAD AHMAD ID: 1231301074
NAMA:ahmed mahmoud ID:243UC245XT
NAME:MOHAMED MUSSA ID:241UC240XY
NAME:Zardawi,ABDULAZEEZ ASHOOR ID:241UC240VM*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

// يتأكد إذا الملف موجود ولا لا
// Checks if the file exists
bool fileExists(const std::string& filename) {
    std::ifstream f(filename.c_str());
    return f.good();
}

// عشان نعرف إذا النص يبدأ بمقطع معيّن
// To check if the string starts with a certain prefix
bool startsWith(const std::string& s, const std::string& p) {
    return (s.size() >= p.size() && s.compare(0, p.size(), p) == 0);
}

// نشيل المسافات من بداية وآخر النص
// Removes leading and trailing whitespace from a string
std::string trim(const std::string& str) {
    std::string r = str; 
    while (!r.empty() && std::isspace((unsigned char)r.front())) r.erase(r.begin());
    while (!r.empty() && std::isspace((unsigned char)r.back())) r.pop_back();
    return r;
}

// نقسم النص حسب محدّد معيّن مثل ','
// Splits the string by a given delimiter
std::vector<std::string> split(const std::string& src, char delimiter) {
    std::vector<std::string> tokens;
    std::string tmp;
    for (char c : src) {
        if (c == delimiter) {
            tokens.push_back(trim(tmp));
            tmp.clear();
        } else {
            tmp.push_back(c);
        }
    }
    if (!tmp.empty()) {
        tokens.push_back(trim(tmp));
    }
    return tokens;
}

int main() {
    // هذا هو الملف اللي بنقرا منه الأوامر
    // File that we read commands from
    std::string dataFile = "data.txt";

    // إذا مهو موجود، ينشئه فاضي
    // If it doesn't exist, create an empty file
    if (!fileExists(dataFile)) {
        std::ofstream createFile(dataFile);
        if (!createFile) {
            std::cerr << "Failed to create " << dataFile << std::endl;
            return 1;
        }
        std::cout << dataFile << " was not found, so it has been created.\n";
        createFile.close();
    }

    // هذي القوائم بتحتفظ بأسماء الداتا بيز والجداول
    // These vectors store the names of databases and tables
    std::vector<std::string> dbs, tables;

    // نفتح الملف للقراءة
    // Open the file for reading
    std::ifstream in(dataFile);
    if (!in.is_open()) {
        std::cerr << "Can't open " << dataFile << std::endl;
        return 1;
    }

    std::cout << "File '" << dataFile << "' opened successfully!\n";

    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (!line.empty() && line.back() == ';') {
            line.pop_back(); // نشيل الفاصلة المنقوطة
                             // remove semicolon
        }

        // CREATE TABLE tableName(...)
        if (startsWith(line, "CREATE TABLE")) {
            // يخلق ملف .tbl ويخزن أعمدة الجدول
            // Creates a .tbl file and stores table columns
            std::string s = trim(line.substr(12));
            auto p1 = s.find('('), p2 = s.rfind(')');
            if (p1 == std::string::npos || p2 == std::string::npos) {
                std::cerr << "Syntax error in CREATE TABLE\n";
                continue;
            }
            std::string tName = trim(s.substr(0, p1));
            std::string cols  = trim(s.substr(p1 + 1, p2 - (p1 + 1)));

            std::ofstream f(tName + ".tbl");
            if (!f.is_open()) {
                std::cerr << "Failed to create table file: " << tName << ".tbl\n";
            } else {
                tables.push_back(tName);
                std::cout << "[OK] Table created: " << tName << std::endl;
                
                // نكتب اسم الجدول والأعمدة بالملف
                // Write table name and columns in the file
                f << "TABLE: " << tName << "\n";

                auto columnsList = split(cols, ',');
                for (auto& c : columnsList) {
                    f << "COLUMN: " << c << "\n";
                }
                f.close();
            }
        }

        // CREATE <dbFile>
        else if (startsWith(line, "CREATE ")) {
            // يعامله كأنه قاعدة بيانات
            // Treats it like a database
            std::string db = trim(line.substr(6));
            std::ofstream f(db);
            if (!f.is_open()) {
                std::cerr << "Failed to create " << db << std::endl;
            } else {
                dbs.push_back(db);
                std::cout << "[OK] Created DB-like file: " << db << std::endl;
                f.close();
            }
        }

        // DATABASES
        else if (line == "DATABASES") {
            // يعرض أسماء الملفات اللي اعتبرناها داتا بيز
            // Lists the files we treated as databases
            std::cout << "> Listing Databases:\n";
            if (dbs.empty()) std::cout << "(No databases found)\n";
            else for (auto& d : dbs) std::cout << d << "\n";
        }

        // TABLES
        else if (line == "TABLES") {
            // يعرض أسماء الجداول اللي بنيناها
            // Lists the tables we have created
            std::cout << "> Listing Tables:\n";
            if (tables.empty()) std::cout << "(No tables found)\n";
            else for (auto& t : tables) std::cout << t << "\n";
        }

        // INSERT INTO tableName(...) VALUES(...)
        else if (startsWith(line, "INSERT INTO")) {
            // يخزن الصف في ملف الجدول
            // Stores a row in the table file
            std::string s = trim(line.substr(11));
            auto p1 = s.find('('), p2 = s.find(')');
            if (p1 == std::string::npos || p2 == std::string::npos) {
                std::cerr << "Syntax error in INSERT INTO\n";
                continue;
            }
            std::string tName = trim(s.substr(0, p1));
            std::string afterCols = trim(s.substr(p2 + 1));
            if (!startsWith(afterCols, "VALUES")) {
                std::cerr << "Syntax error: missing VALUES\n";
                continue;
            }
            auto ps = afterCols.find('('), pe = afterCols.rfind(')');
            if (ps == std::string::npos || pe == std::string::npos) {
                std::cerr << "Syntax error in INSERT: missing parentheses in VALUES\n";
                continue;
            }
            std::string vals = trim(afterCols.substr(ps + 1, pe - (ps + 1)));
            std::string cols = trim(s.substr(p1 + 1, p2 - (p1 + 1)));

            std::ofstream f(tName + ".tbl", std::ios::app);
            if (!f.is_open()) {
                std::cerr << "Could not open table file: " << tName << ".tbl\n";
                continue;
            }
            std::cout << "[OK] Inserted into table: " << tName << std::endl;
            f << "ROW: [COLUMNS] " << cols << " [VALUES] " << vals << "\n";
            f.close();
        }

        // SELECT * FROM tableName
        else if (startsWith(line, "SELECT * FROM")) {
            // يعرض محتويات الجدول أو يعدّ الصفوف
            // Displays table contents or counts rows
            std::string tName = trim(line.substr(13));
            if (startsWith(tName, "COUNT(*) FROM")) {
                // مثال: SELECT COUNT(*) FROM customer
                std::string rest = trim(tName.substr(11)); 
                std::string tbl = trim(rest);
                std::string tblPath = tbl + ".tbl";
                std::ifstream inTbl(tblPath);
                if (!inTbl.is_open()) {
                    std::cerr << "Could not open table file: " << tblPath << std::endl;
                    continue;
                }
                int rowCount = 0;
                std::string ln;
                while (std::getline(inTbl, ln)) {
                    if (startsWith(ln, "ROW:")) {
                        rowCount++;
                    }
                }
                inTbl.close();
                std::cout << rowCount << std::endl; 
            } else {
                // SELECT * FROM tableName
                std::ifstream f(tName + ".tbl");
                if (!f.is_open()) {
                    std::cerr << "Could not open table file: " << tName << ".tbl\n";
                    continue;
                }
                std::cout << "> SELECT * FROM " << tName << "\n";
                std::string row;
                while (std::getline(f, row)) {
                    std::cout << row << "\n";
                }
                f.close();
            }
        }

        // UPDATE tableName SET col=val,... WHERE col2=val2
        else if (startsWith(line, "UPDATE ")) {
            // يعدّل قيم الصفوف المطابقة للشرط
            // Updates row values matching the WHERE condition
            std::string s = trim(line.substr(6)); 
            auto sp = s.find(' ');
            if (sp == std::string::npos) {
                std::cerr << "Syntax error in UPDATE: missing tableName\n";
                continue;
            }
            std::string tName = trim(s.substr(0, sp));
            std::string rest = trim(s.substr(sp + 1));
            if (!startsWith(rest, "SET ")) {
                std::cerr << "Syntax error in UPDATE: missing 'SET'\n";
                continue;
            }
            rest = trim(rest.substr(4)); 
            auto whPos = rest.find("WHERE");
            if (whPos == std::string::npos) {
                std::cerr << "Syntax error in UPDATE: missing WHERE clause\n";
                continue;
            }
            std::string setPart = trim(rest.substr(0, whPos));
            std::string wherePart = trim(rest.substr(whPos + 5));

            auto setsVec = split(setPart, ',');
            auto eqPos = wherePart.find('=');
            if (eqPos == std::string::npos) {
                std::cerr << "Syntax error in UPDATE: invalid WHERE col=val\n";
                continue;
            }
            std::string wCol = trim(wherePart.substr(0, eqPos));
            std::string wVal = trim(wherePart.substr(eqPos + 1));

            std::string tblPath = tName + ".tbl";
            std::ifstream inTbl(tblPath);
            if (!inTbl.is_open()) {
                std::cerr << "Could not open table file: " << tblPath << std::endl;
                continue;
            }
            std::vector<std::string> fileLines;
            {
                std::string tempLn;
                while (std::getline(inTbl, tempLn)) {
                    fileLines.push_back(tempLn);
                }
            }
            inTbl.close();

            bool updatedAny = false;
            for (auto& l : fileLines) {
                if (startsWith(l, "ROW:")) {
                    // ROW: [COLUMNS] col1,col2 [VALUES] val1,val2
                    auto rowPart = trim(l.substr(4)); 
                    auto valPos = rowPart.find("[VALUES]");
                    if (valPos == std::string::npos) continue;
                    std::string colsStr = trim(rowPart.substr(9, valPos - 9));
                    std::string valsStr = trim(rowPart.substr(valPos + 8));

                    auto colsList = split(colsStr, ',');
                    auto valsList = split(valsStr, ',');

                    if (colsList.size() != valsList.size()) continue;

                    bool match = false;
                    for (size_t i = 0; i < colsList.size(); i++) {
                        auto cName = trim(colsList[i]);
                        auto cVal  = trim(valsList[i]);
                        if (cName == wCol && cVal == wVal) {
                            match = true;
                            break;
                        }
                    }

                    if (match) {
                        for (auto& sPart : setsVec) {
                            auto eqp = sPart.find('=');
                            if (eqp == std::string::npos) continue;
                            std::string setCol = trim(sPart.substr(0, eqp));
                            std::string setVal = trim(sPart.substr(eqp + 1));
                            for (size_t i = 0; i < colsList.size(); i++) {
                                if (trim(colsList[i]) == setCol) {
                                    valsList[i] = setVal;
                                }
                            }
                        }
                        std::string newLine = "ROW: [COLUMNS] " + colsList[0];
                        for (size_t i = 1; i < colsList.size(); i++) {
                            newLine += "," + colsList[i];
                        }
                        newLine += " [VALUES] " + valsList[0];
                        for (size_t i = 1; i < valsList.size(); i++) {
                            newLine += "," + valsList[i];
                        }
                        l = newLine;
                        updatedAny = true;
                    }
                }
            }

            if (updatedAny) {
                std::ofstream outTbl(tblPath, std::ios::trunc);
                if (!outTbl.is_open()) {
                    std::cerr << "Could not open table file to rewrite after update.\n";
                    continue;
                }
                for (auto& ll : fileLines) {
                    outTbl << ll << "\n";
                }
                outTbl.close();
                std::cout << "[OK] Rows updated in table: " << tName << std::endl;
            } else {
                std::cout << "[INFO] No rows matched the WHERE condition.\n";
            }
        }

        // DELETE FROM tableName WHERE col=val
        else if (startsWith(line, "DELETE FROM")) {
            // يحذف الصفوف اللي ينطبق عليها الشرط
            // Deletes rows matching the WHERE condition
            std::string s = trim(line.substr(11));
            auto sp = s.find("WHERE");
            if (sp == std::string::npos) {
                std::cerr << "Syntax error in DELETE: missing WHERE\n";
                continue;
            }
            std::string tName = trim(s.substr(0, sp));
            std::string wPart = trim(s.substr(sp + 5));
            auto eqPos = wPart.find('=');
            if (eqPos == std::string::npos) {
                std::cerr << "Syntax error in DELETE: missing = in WHERE\n";
                continue;
            }
            std::string wCol = trim(wPart.substr(0, eqPos));
            std::string wVal = trim(wPart.substr(eqPos + 1));

            std::string tblPath = tName + ".tbl";
            std::ifstream inTbl(tblPath);
            if (!inTbl.is_open()) {
                std::cerr << "Could not open table file: " << tblPath << std::endl;
                continue;
            }
            std::vector<std::string> fileLines;
            {
                std::string tmpLine;
                while (std::getline(inTbl, tmpLine)) {
                    fileLines.push_back(tmpLine);
                }
            }
            inTbl.close();

            bool deletedAny = false;
            for (auto & l : fileLines) {
                if (startsWith(l, "ROW:")) {
                    std::string rowPart = trim(l.substr(4));
                    auto valPos = rowPart.find("[VALUES]");
                    if (valPos == std::string::npos) continue;
                    std::string colsStr = trim(rowPart.substr(9, valPos - 9));
                    std::string valsStr = trim(rowPart.substr(valPos + 8));
                    auto colsList = split(colsStr, ',');
                    auto valsList = split(valsStr, ',');

                    if (colsList.size() != valsList.size()) continue;

                    bool match = false;
                    for (size_t i = 0; i < colsList.size(); i++) {
                        auto cName = trim(colsList[i]);
                        auto cVal  = trim(valsList[i]);
                        if (cName == wCol && cVal == wVal) {
                            match = true;
                            break;
                        }
                    }

                    if (match) {
                        // نخلي السطر فاضي عشان نحذفه
                        // Clear the line to remove it
                        l.clear();
                        deletedAny = true;
                    }
                }
            }

            if (deletedAny) {
                std::ofstream outTbl(tblPath, std::ios::trunc);
                if (!outTbl.is_open()) {
                    std::cerr << "Could not open table file to rewrite after delete.\n";
                    continue;
                }
                for (auto & ll : fileLines) {
                    if (!ll.empty()) {
                        outTbl << ll << "\n";
                    }
                }
                outTbl.close();
                std::cout << "[OK] Rows deleted from table: " << tName << std::endl;
            } else {
                std::cout << "[INFO] No rows matched the WHERE condition for DELETE.\n";
            }
        }

        else {
            // أمر ما تعرّفنا عليه
            // Unrecognized command
            std::cout << "[WARN] Unrecognized command: " << line << std::endl;
        }
    }

    in.close();
    return 0;
}
