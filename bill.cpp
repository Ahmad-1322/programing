#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

// يتحقّق إن كان الملف موجودًا
bool fileExists(const std::string& filename) {
    std::ifstream f(filename.c_str());
    return f.good();
}

// لمعرفة إن كان النص يبدأ بمقطع معيّن
bool startsWith(const std::string& s, const std::string& p) {
    return (s.size() >= p.size() && s.compare(0, p.size(), p) == 0);
}

// لحذف المسافات في بداية ونهاية السلسلة
std::string trim(const std::string& str) {
    std::string r = str; 
    while (!r.empty() && std::isspace((unsigned char)r.front())) r.erase(r.begin());
    while (!r.empty() && std::isspace((unsigned char)r.back())) r.pop_back();
    return r;
}

// يفصل النص حسب محدّد معيّن (مثل ",")
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
    // الملف الذي سنقرأ منه الأوامر
    std::string dataFile = "data.txt";

    // إن لم يكن موجودًا، ننشئه فارغًا
    if (!fileExists(dataFile)) {
        std::ofstream createFile(dataFile);
        if (!createFile) {
            std::cerr << "Failed to create " << dataFile << std::endl;
            return 1;
        }
        std::cout << dataFile << " was not found, so it has been created.\n";
        createFile.close();
    }

    // قائمات لتخزين أسماء الملفات والقواعد والجداول
    std::vector<std::string> dbs, tables;

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
            line.pop_back(); // إزالة الفاصلة المنقوطة بالآخر
        }

        // CREATE TABLE tableName(...)
        if (startsWith(line, "CREATE TABLE")) {
            // مثال: CREATE TABLE customer(customer_id INT, customer_name TEXT,...)
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
                f << "TABLE: " << tName << "\n";
                // تخزين الأعمدة في الملف كل سطر يبدأ بـ "COLUMN:"
                auto columnsList = split(cols, ',');
                for (auto& c : columnsList) {
                    f << "COLUMN: " << c << "\n";
                }
                f.close();
            }
        }

        // CREATE <dbFile>
        else if (startsWith(line, "CREATE ")) {
            // يعامله كأنه إنشاء قاعدة
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
            std::cout << "> Listing Databases:\n";
            if (dbs.empty()) std::cout << "(No databases found)\n";
            else for (auto& d : dbs) std::cout << d << "\n";
        }

        // TABLES
        else if (line == "TABLES") {
            std::cout << "> Listing Tables:\n";
            if (tables.empty()) std::cout << "(No tables found)\n";
            else for (auto& t : tables) std::cout << t << "\n";
        }

        // INSERT INTO tableName(...) VALUES(...)
        else if (startsWith(line, "INSERT INTO")) {
            // مثال: INSERT INTO customer(col1,col2,...) VALUES(val1,val2,...)
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
            // نبحث عن ( ) بعد VALUES
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
            // نسجّل الصف في سطر يبدأ بـ "ROW:"
            f << "ROW: [COLUMNS] " << cols << " [VALUES] " << vals << "\n";
            f.close();
        }

        // SELECT * FROM tableName
        else if (startsWith(line, "SELECT * FROM")) {
            // مثال: SELECT * FROM customer
            std::string tName = trim(line.substr(13));
            // فحص أوامر خاصة: SELECT COUNT(*) FROM ...
            if (startsWith(tName, "COUNT(*) FROM")) {
                // صيغة: SELECT COUNT(*) FROM tableName
                // نفصل "COUNT(*) FROM "
                std::string rest = trim(tName.substr(11)); 
                // rest هو اسم الجدول
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
            // مثال: UPDATE customer SET customer_email='email333' WHERE customer_id=3
            std::string s = trim(line.substr(6)); 
            // اسحب اسم الجدول
            auto sp = s.find(' ');
            if (sp == std::string::npos) {
                std::cerr << "Syntax error in UPDATE: missing tableName\n";
                continue;
            }
            std::string tName = trim(s.substr(0, sp));
            std::string rest = trim(s.substr(sp + 1));
            // نتوقع: SET ...
            if (!startsWith(rest, "SET ")) {
                std::cerr << "Syntax error in UPDATE: missing 'SET'\n";
                continue;
            }
            rest = trim(rest.substr(4)); 
            // ابحث عن WHERE
            auto whPos = rest.find("WHERE");
            if (whPos == std::string::npos) {
                std::cerr << "Syntax error in UPDATE: missing WHERE clause\n";
                continue;
            }
            std::string setPart = trim(rest.substr(0, whPos));
            std::string wherePart = trim(rest.substr(whPos + 5));

            // نفصل setPart بفواصل
            // شكل col=val,col2=val2
            auto setsVec = split(setPart, ',');
            // wherePart يفترض col=val
            auto eqPos = wherePart.find('=');
            if (eqPos == std::string::npos) {
                std::cerr << "Syntax error in UPDATE: invalid WHERE col=val\n";
                continue;
            }
            std::string wCol = trim(wherePart.substr(0, eqPos));
            std::string wVal = trim(wherePart.substr(eqPos + 1));

            // نقرأ الجدول كامل ونعدّل
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
                    // نفصل جزئيًا للحصول على col=val
                    // لكننا سنحولها لأسلوب "colName=valName"
                    auto rowPart = trim(l.substr(4)); 
                    // نقسمه لنصين: [COLUMNS]... و[VALUES]...
                    // ابسط طريقة: ابحث عن "[VALUES]"
                    auto valPos = rowPart.find("[VALUES]");
                    if (valPos == std::string::npos) continue;
                    // استخراج أعمدة وقيم
                    std::string colsStr = trim(rowPart.substr(9, valPos - 9)); // بعد [COLUMNS]
                    std::string valsStr = trim(rowPart.substr(valPos + 8));   // بعد [VALUES]
                    // ممكن تحتوي valsStr على قيم
                    // نزيل بدايته لو كان فيه مسافات
                    valsStr = trim(valsStr);

                    auto colsList = split(colsStr, ',');
                    auto valsList = split(valsStr, ',');

                    if (colsList.size() != valsList.size()) {
                        // تجاهل هذا الصف
                        continue;
                    }

                    // نشيك هل هذا الصف يطابق ال WHERE
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
                        // طبق التحديث
                        for (auto& sPart : setsVec) {
                            auto eqp = sPart.find('=');
                            if (eqp == std::string::npos) continue;
                            std::string setCol = trim(sPart.substr(0, eqp));
                            std::string setVal = trim(sPart.substr(eqp + 1));
                            // عدّل في valsList إن طابق الاسم
                            for (size_t i = 0; i < colsList.size(); i++) {
                                if (trim(colsList[i]) == setCol) {
                                    valsList[i] = setVal;
                                }
                            }
                        }
                        // إعادة بناء السطر
                        // ROW: [COLUMNS] col1,col2 [VALUES] val1,val2
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
                // إعادة الكتابة
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
            // مثال: DELETE FROM customer WHERE customer_id=4
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
                        // امسح السطر
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
            std::cout << "[WARN] Unrecognized command: " << line << std::endl;
        }
    }

    in.close();
    return 0;
}
