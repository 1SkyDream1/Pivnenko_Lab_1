#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <vector>
#include <ctime>

using namespace std;

struct Pipe {
    int id;
    string name;
    double length = 0.0;
    int diameter = 0;
    bool under_repair = false;
    bool created = false;
    static int next_id;

    Pipe() {
        id = ++next_id;
    }
    void display() const {
        cout << "[" << id << "] " << name
            << " | длина: " << length << " км"
            << " | диаметр: " << diameter << " мм"
            << " | статус: " << (under_repair ? "в ремонте" : "работает") << "\n";
    }
    bool matchesNameFilter(const string& search_name) const {
        if (search_name.empty()) return true;
        return name.find(search_name) != string::npos;
    }

    bool matchesRepairFilter(bool filter_by_repair, bool repair_status) const {
        if (!filter_by_repair) return true;
        return under_repair == repair_status;
    }
};

int Pipe::next_id = 0; // инициализация статического счётчика

struct CS {
    int id;
    string name;
    int workshop_count = 0;
    int working_workshop_count = 0;
    string station_class;
    bool created = false;
    static int next_id;

    CS() {
        id = ++next_id;
    }
    void display() const {
        cout << "[" << id << "] " << name
            << " | цехов: " << workshop_count
            << " | работающих: " << working_workshop_count
            << " | класс: " << station_class
            << " | процент незадействованных: " << getUnusedPercentage() << "%\n";
    }
    bool matchesNameFilter(const string& search_name) const {
        if (search_name.empty()) return true;
        return name.find(search_name) != string::npos;
    }

    double getUnusedPercentage() const {
        if (workshop_count == 0) return 0.0;
        return ((workshop_count - working_workshop_count) * 100.0) / workshop_count;
    }

    bool matchesUnusedFilter(bool filter_by_unused, double min_percent) const {
        if (!filter_by_unused) return true;
        return getUnusedPercentage() >= min_percent;
    }
};

int CS::next_id = 0;

// ------------------ Утилиты ------------------

// логирование действий
void logAction(const string& action, const string& filename = "log.txt") {
    ofstream fout(filename, ios::app);
    if (!fout.is_open()) return;
    time_t t = time(nullptr);
    struct tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    fout << "[" << buf << "] " << action << "\n";
    fout.close();
}

// удаление лишних пробелов
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == string::npos) return "";

    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

// содержит ли строка только буквы и цифры (и пробел, дефис, подчеркивание)
bool isAlphanumeric(const string& str) {
    for (char c : str) {
        if (!isalnum(static_cast<unsigned char>(c)) && c != ' ' && c != '-' && c != '_') {
            return false;
        }
    }
    return true;
}
// ПРОСТАЯ РАБОЧАЯ версия для меню
int getIntegerInput(const string& prompt, int min_val = 0, int max_val = numeric_limits<int>::max()) {
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        input = trim(input);

        if (input.empty()) {
            cout << "Ошибка! Ввод не может быть пустым. Попробуйте снова.\n";
            continue;
        }

        // Пробуем преобразовать в число
        try {
            int value = stoi(input);

            if (value < min_val) {
                cout << "Ошибка! Значение должно быть не меньше " << min_val << ". Попробуйте снова.\n";
            }
            else if (value > max_val) {
                cout << "Ошибка! Значение должно быть не больше " << max_val << ". Попробуйте снова.\n";
            }
            else {
                return value;
            }
        }
        catch (const exception& e) {
            cout << "Ошибка! Введите корректное целое число. Попробуйте снова.\n";
        }
    }
}

// безопасное чтение дробного числа
double getDoubleInput(const string& prompt, double min_val = 0.001, double max_val = 1e9) {
    double value;
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        input = trim(input);

        if (input.empty()) {
            cout << "Ошибка! Ввод не может быть пустым. Попробуйте снова.\n";
            continue;
        }

        bool has_invalid_char = false;
        bool has_dot = false;
        for (size_t i = 0; i < input.size(); i++) {
            if (input[i] == '.') {
                if (has_dot) {
                    has_invalid_char = true; // больше одной точки
                    break;
                }
                has_dot = true;
                continue;
            }
            if (!isdigit(static_cast<unsigned char>(input[i]))) {
                has_invalid_char = true;
                break;
            }
        }

        if (has_invalid_char) {
            cout << "Ошибка! Введите число. Попробуйте снова: \n";
            continue;
        }

        stringstream ss(input);
        if (ss >> value) {
            if (value < min_val) {
                cout << "Ошибка! Значение должно быть не меньше " << min_val << ". Попробуйте снова.\n";
            }
            else if (value > max_val) {
                cout << "Ошибка! Значение должно быть не больше " << max_val << ". Попробуйте снова.\n";
            }
            else {
                return value;
            }
        }
        else {
            cout << "Ошибка! Введите корректное число. Попробуйте снова.\n";
        }
    }
}

// безопасное чтения строки
string getStringInput(const string& prompt, bool allow_empty = false, bool alphanumeric_only = false) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        value = trim(value);

        if (value.empty() && !allow_empty) {
            cout << "Ошибка! Ввод не может быть пустым. Попробуйте снова: \n";
            continue;
        }

        if (alphanumeric_only && !value.empty() && !isAlphanumeric(value)) {
            cout << "Ошибка! Название может содержать только буквы, цифры, пробелы, дефисы и подчеркивания. Попробуйте снова: \n";
            continue;
        }

        return value;
    }
}

// безопасное чтение да/нет
bool getYesNoInput(const string& prompt) {
    while (true) {
        string input = getStringInput(prompt, false, true);
        transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (input == "да" || input == "д" || input == "yes" || input == "y" || input == "1" || input == "true") {
            return true;
        }
        else if (input == "нет" || input == "н" || input == "no" || input == "n" || input == "0" || input == "false") {
            return false;
        }
        else {
            cout << "Ошибка! Пожалуйста, введите 'да' или 'нет': \n";
        }
    }
}

// ------------------ Операции для множественных объектов (трубы) ------------------

void addPipe(vector<Pipe>& pipes) {
    cout << "\n=== Добавление новой трубы ===\n";
    Pipe p;
    p.name = getStringInput("Введите название трубы: ", false, true);
    p.length = getDoubleInput("Введите длину трубы (км): ", 0.001, 10000);
    p.diameter = getIntegerInput("Введите диаметр трубы (мм): ", 10, 2000);
    p.under_repair = false;
    p.created = true;
    pipes.push_back(p);

    string msg = "Добавлена труба [ID=" + to_string(p.id) + "]: " + p.name;
    cout << "Труба '" << p.name << "' успешно добавлена! (ID=" << p.id << ")\n";
    logAction(msg);
}

void showPipes(const vector<Pipe>& pipes) {
    cout << "\n=== Список труб ===\n";
    if (pipes.empty()) {
        cout << "Труб нет.\n";
        return;
    }
    for (const Pipe& p : pipes) {
        cout << "[" << p.id << "] " << p.name
            << " | длина: " << p.length << " км"
            << " | диаметр: " << p.diameter << " мм"
            << " | статус: " << (p.under_repair ? "в ремонте" : "работает") << "\n";
    }
}

void editPipeAt(vector<Pipe>& pipes) {
    if (pipes.empty()) {
        cout << "Нет труб для редактирования.\n";
        return;
    }
    showPipes(pipes);
    int idx = getIntegerInput("Введите номер трубы для редактирования (0 - отмена): ", 0, static_cast<int>(pipes.size()));
    if (idx == 0) {
        cout << "Отмена редактирования.\n";
        return;
    }
    Pipe& p = pipes[idx - 1];
    cout << "\n=== Редактирование трубы '" << p.name << "' ===\n";
    cout << "Длина: " << p.length << " км (неизменяемый параметр)\n";
    cout << "Диаметр: " << p.diameter << " мм (неизменяемый параметр)\n";
    cout << "Название: " << p.name << " (неизменяемый параметр)\n\n";

    bool changeStatus = getYesNoInput("Изменить статус ремонта? (да/нет): ");
    if (changeStatus) {
        p.under_repair = !p.under_repair;
        cout << "Статус ремонта изменен! Труба теперь " << (p.under_repair ? "в ремонте" : "работает") << endl;
        logAction("Изменён статус ремонта трубы: " + p.name + " -> " + (p.under_repair ? "в ремонте" : "работает"));
    }
    else {
        cout << "Статус не изменён.\n";
    }
}

void deletePipeAt(vector<Pipe>& pipes) {
    if (pipes.empty()) {
        cout << "Нет труб для удаления.\n";
        return;
    }
    showPipes(pipes);
    int idx = getIntegerInput("Введите номер трубы для удаления (0 - отмена): ", 0, static_cast<int>(pipes.size()));
    if (idx == 0) {
        cout << "Отмена удаления.\n";
        return;
    }
    Pipe p = pipes[idx - 1]; // копия для логирования
    bool confirm = getYesNoInput("Подтвердите удаление трубы '" + p.name + "' (да/нет): ");
    if (confirm) {
        pipes.erase(pipes.begin() + (idx - 1));
        cout << "Труба удалена.\n";
        logAction("Удалена труба: " + p.name);
    }
    else {
        cout << "Удаление отменено.\n";
    }
}

// ------------------ Операции для множественных объектов (КС) ------------------

void addCS(vector<CS>& css) {
    cout << "\n=== Добавление новой КС ===\n";
    CS cs;
    cs.name = getStringInput("Введите название КС: ", false, true);
    cs.workshop_count = getIntegerInput("Введите количество цехов: ", 1, 100);
    cs.working_workshop_count = getIntegerInput("Введите количество работающих цехов: ", 0, cs.workshop_count);
    cs.station_class = getStringInput("Введите класс станции: ", false, true);
    cs.created = true;
    css.push_back(cs);

    string msg = "Добавлена КС [ID=" + to_string(cs.id) + "]: " + cs.name;
    cout << "КС '" << cs.name << "' успешно добавлена! (ID=" << cs.id << ")\n";
    logAction(msg);
}

void showCSs(const vector<CS>& css) {
    cout << "\n=== Список КС ===\n";
    if (css.empty()) {
        cout << "КС нет.\n";
        return;
    }
    for (const CS& c : css) {
        cout << "[" << c.id << "] " << c.name
            << " | цехов: " << c.workshop_count
            << " | работающих: " << c.working_workshop_count
            << " | класс: " << c.station_class << "\n";
    }
}


void editCSAt(vector<CS>& css) {
    if (css.empty()) {
        cout << "Нет КС для редактирования.\n";
        return;
    }
    showCSs(css);
    int idx = getIntegerInput("Введите номер КС для редактирования (0 - отмена): ", 0, static_cast<int>(css.size()));
    if (idx == 0) {
        cout << "Отмена редактирования.\n";
        return;
    }
    CS& cs = css[idx - 1];

    cout << "\n=== Редактирование КС '" << cs.name << "' ===\n";
    cout << "Название: " << cs.name << " (неизменяемый параметр)\n";
    cout << "Всего цехов: " << cs.workshop_count << " (неизменяемый параметр)\n";
    cout << "Класс станции: " << cs.station_class << " (неизменяемый параметр)\n";
    cout << "Текущее количество работающих цехов: " << cs.working_workshop_count << endl;

    cout << "\nВыберите действие:\n"
        << "1 - Запустить цех\n"
        << "2 - Остановить цех\n"
        << "0 - Отмена\n";

    int choice = getIntegerInput("Ваш выбор: ", 0, 2);

    if (choice == 1) {
        if (cs.working_workshop_count < cs.workshop_count) {
            cs.working_workshop_count++;
            cout << "Цех запущен! Теперь работает " << cs.working_workshop_count << " из " << cs.workshop_count << " цехов.\n";
            logAction("Запущен цех на КС: " + cs.name + " -> " + to_string(cs.working_workshop_count) + "/" + to_string(cs.workshop_count));
        }
        else {
            cout << "Ошибка! Все цехи уже работают.\n";
        }
    }
    else if (choice == 2) {
        if (cs.working_workshop_count > 0) {
            cs.working_workshop_count--;
            cout << "Цех остановлен! Теперь работает " << cs.working_workshop_count << " из " << cs.workshop_count << " цехов.\n";
            logAction("Остановлен цех на КС: " + cs.name + " -> " + to_string(cs.working_workshop_count) + "/" + to_string(cs.workshop_count));
        }
        else {
            cout << "Ошибка! Нет работающих цехов для остановки.\n";
        }
    }
    else {
        cout << "Редактирование отменено.\n";
    }
}

void deleteCSAt(vector<CS>& css) {
    if (css.empty()) {
        cout << "Нет КС для удаления.\n";
        return;
    }
    showCSs(css);
    int idx = getIntegerInput("Введите номер КС для удаления (0 - отмена): ", 0, static_cast<int>(css.size()));
    if (idx == 0) {
        cout << "Отмена удаления.\n";
        return;
    }
    CS cs = css[idx - 1]; // копия для логирования
    bool confirm = getYesNoInput("Подтвердите удаление КС '" + cs.name + "' (да/нет): ");
    if (confirm) {
        css.erase(css.begin() + (idx - 1));
        cout << "КС удалена.\n";
        logAction("Удалена КС: " + cs.name);
    }
    else {
        cout << "Удаление отменено.\n";
    }
}

// ------------------ Поиск труб ------------------
void searchPipes(const vector<Pipe>& pipes) {
    cout << "\n=== Поиск труб ===\n";

    string name_filter = getStringInput("Введите название для поиска (или Enter для пропуска): ", true, true);
    bool use_repair_filter = getYesNoInput("Фильтровать по статусу ремонта? (да/нет): ");

    bool repair_status = false;
    if (use_repair_filter) {
        repair_status = getYesNoInput("Искать трубы в ремонте? (да/нет): ");
    }

    cout << "\n=== Результаты поиска ===\n";
    bool found = false;
    for (const Pipe& p : pipes) {
        if (p.matchesNameFilter(name_filter) && p.matchesRepairFilter(use_repair_filter, repair_status)) {
            p.display();
            found = true;
        }
    }

    if (!found) {
        cout << "Трубы по заданным критериям не найдены.\n";
    }
}

// ------------------ Поиск КС ------------------
void searchCSs(const vector<CS>& css) {
    cout << "\n=== Поиск КС ===\n";

    string name_filter = getStringInput("Введите название для поиска (или Enter для пропуска): ", true, true);
    bool use_unused_filter = getYesNoInput("Фильтровать по проценту незадействованных цехов? (да/нет): ");

    double min_percent = 0.0;
    if (use_unused_filter) {
        min_percent = getDoubleInput("Введите минимальный процент незадействованных цехов: ", 0.0, 100.0);
    }

    cout << "\n=== Результаты поиска ===\n";
    bool found = false;
    for (const CS& c : css) {
        if (c.matchesNameFilter(name_filter) && c.matchesUnusedFilter(use_unused_filter, min_percent)) {
            c.display();
            found = true;
        }
    }

    if (!found) {
        cout << "КС по заданным критериям не найдены.\n";
    }
}

// ------------------ Пакетное редактирование труб ------------------
void batchEditPipes(vector<Pipe>& pipes) {
    cout << "\n=== Пакетное редактирование труб ===\n";

    // Сначала ищем трубы по фильтру
    string name_filter = getStringInput("Введите название для поиска (или Enter для всех): ", true, true);
    bool use_repair_filter = getYesNoInput("Фильтровать по статусу ремонта? (да/нет): ");

    bool repair_status = false;
    if (use_repair_filter) {
        repair_status = getYesNoInput("Искать трубы в ремонте? (да/нет): ");
    }

    // Собираем подходящие трубы
    vector<int> matching_indices;
    for (size_t i = 0; i < pipes.size(); i++) {
        if (pipes[i].matchesNameFilter(name_filter) &&
            pipes[i].matchesRepairFilter(use_repair_filter, repair_status)) {
            matching_indices.push_back(i);
        }
    }

    if (matching_indices.empty()) {
        cout << "Трубы по заданным критериям не найдены.\n";
        return;
    }

    // Показываем найденные трубы
    cout << "\n=== Найдены трубы для редактирования ===\n";
    for (int idx : matching_indices) {
        cout << "[" << (idx + 1) << "] ";
        pipes[idx].display();
    }

    // Выбор режима редактирования
    cout << "\nРежимы редактирования:\n"
        << "1 - Изменить все найденные трубы\n"
        << "2 - Выбрать конкретные трубы\n"
        << "0 - Отмена\n";

    int mode = getIntegerInput("Выберите режим: ", 0, 2);

    if (mode == 0) {
        cout << "Отмена пакетного редактирования.\n";
        return;
    }

    vector<int> selected_indices;
    if (mode == 1) {
        // Выбраны все найденные трубы
        selected_indices = matching_indices;
    }
    else if (mode == 2) {
        // Пользователь выбирает конкретные трубы
        cout << "\nВведите номера труб для редактирования (через пробел, 0 - завершить ввод):\n";
        string input;
        getline(cin, input);
        stringstream ss(input);
        int num;

        while (ss >> num) {
            if (num == 0) break;
            // Проверяем, что номер в пределах найденных труб
            bool valid = false;
            for (int idx : matching_indices) {
                if (idx + 1 == num) {
                    selected_indices.push_back(idx);
                    valid = true;
                    break;
                }
            }
            if (!valid) {
                cout << "Труба с номером " << num << " не найдена в результатах поиска.\n";
            }
        }

        if (selected_indices.empty()) {
            cout << "Не выбрано ни одной трубы для редактирования.\n";
            return;
        }
    }

    // Выбор действия для редактирования
    cout << "\nВыберите действие:\n"
        << "1 - Перевести в ремонт\n"
        << "2 - Вывести из ремонта\n"
        << "3 - Переключить статус ремонта\n"
        << "0 - Отмена\n";

    int action = getIntegerInput("Ваш выбор: ", 0, 3);

    if (action == 0) {
        cout << "Отмена редактирования.\n";
        return;
    }

    // Применяем изменения
    int changed_count = 0;
    for (int idx : selected_indices) {
        Pipe& pipe = pipes[idx];
        bool original_status = pipe.under_repair;
        bool new_status = original_status;

        switch (action) {
        case 1: new_status = true; break;
        case 2: new_status = false; break;
        case 3: new_status = !original_status; break;
        }

        if (original_status != new_status) {
            pipe.under_repair = new_status;
            changed_count++;
            cout << "Труба '" << pipe.name << "' -> " << (new_status ? "в ремонте" : "работает") << "\n";
        }
    }

    cout << "\nИзменено труб: " << changed_count << "\n";

    // Логирование
    string log_msg = "Пакетное редактирование: изменено " + to_string(changed_count) + " труб";
    if (!name_filter.empty()) log_msg += ", фильтр: '" + name_filter + "'";
    if (use_repair_filter) log_msg += ", статус: " + string(repair_status ? "в ремонте" : "работает");
    logAction(log_msg);
}

// ------------------ Сохранение и загрузка ------------------

void savePipesToFile(const vector<Pipe>& pipes, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для сохранения труб: " << filename << endl;
        return;
    }

    // Сохраняем текущий next_id первой строкой
    file << Pipe::next_id << endl;

    for (const Pipe& p : pipes) {
        file << p.id << "|" << p.name << "|" << p.length << "|"
            << p.diameter << "|" << p.under_repair << endl;
    }

    file.close();
    cout << "Трубы сохранены в файл: " << filename << " (записей: " << pipes.size() << ")" << endl;
    logAction("Сохранение труб в файл: " + filename + ", записей: " + to_string(pipes.size()));
}

void saveCSToFile(const vector<CS>& css, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для сохранения КС: " << filename << endl;
        return;
    }

    // Сохраняем текущий next_id первой строкой
    file << CS::next_id << endl;

    for (const CS& c : css) {
        file << c.id << "|" << c.name << "|" << c.workshop_count << "|"
            << c.working_workshop_count << "|" << c.station_class << endl;
    }

    file.close();
    cout << "КС сохранены в файл: " << filename << " (записей: " << css.size() << ")" << endl;
    logAction("Сохранение КС в файл: " + filename + ", записей: " + to_string(css.size()));
}

void saveData(const vector<Pipe>& pipes, const vector<CS>& css) {
    cout << "\n=== Сохранение данных ===\n";

    // Проверяем, есть ли данные для сохранения
    if (pipes.empty() && css.empty()) {
        cout << "Ошибка! Нет данных для сохранения.\n";
        cout << "Добавьте трубы или КС перед сохранением.\n";
        logAction("Попытка сохранения при отсутствии данных");
        return;
    }

    string pipe_filename = getStringInput("Введите имя файла для труб (по умолчанию pipes.txt): ", true, false);
    if (pipe_filename.empty()) pipe_filename = "pipes.txt";

    string cs_filename = getStringInput("Введите имя файла для КС (по умолчанию cs.txt): ", true, false);
    if (cs_filename.empty()) cs_filename = "cs.txt";

    // Сохраняем только если есть данные
    if (!pipes.empty()) {
        savePipesToFile(pipes, pipe_filename);
    }
    else {
        cout << "Труб нет, пропускаем сохранение труб.\n";
    }

    if (!css.empty()) {
        saveCSToFile(css, cs_filename);
    }
    else {
        cout << "КС нет, пропускаем сохранение КС.\n";
    }

    cout << "Сохранение завершено!\n";
}

void loadPipesFromFile(vector<Pipe>& pipes, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для загрузки труб: " << filename << endl;
        return;
    }

    // Читаем next_id первой строкой
    string next_id_str;
    if (getline(file, next_id_str)) {
        try {
            Pipe::next_id = stoi(next_id_str);
        }
        catch (...) {
            cout << "Ошибка чтения next_id из файла" << endl;
        }
    }

    int loaded_count = 0;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        vector<string> tokens;

        while (getline(ss, token, '|')) {
            tokens.push_back(token);
        }

        if (tokens.size() == 5) {
            try {
                int id = stoi(tokens[0]);
                string name = tokens[1];
                double length = stod(tokens[2]);
                int diameter = stoi(tokens[3]);
                bool under_repair = (tokens[4] == "1");

                // Создаем трубу с существующим ID
                Pipe p;
                p.id = id;
                p.name = name;
                p.length = length;
                p.diameter = diameter;
                p.under_repair = under_repair;
                p.created = true;

                pipes.push_back(p);
                loaded_count++;

            }
            catch (...) {
                cout << "Ошибка разбора строки трубы: " << line << endl;
            }
        }
    }

    file.close();
    cout << "Трубы загружены из файла: " << filename << " (записей: " << loaded_count << ")" << endl;
    logAction("Загрузка труб из файла: " + filename + ", записей: " + to_string(loaded_count));
}

void loadCSFromFile(vector<CS>& css, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для загрузки КС: " << filename << endl;
        return;
    }

    // Читаем next_id первой строкой
    string next_id_str;
    if (getline(file, next_id_str)) {
        try {
            CS::next_id = stoi(next_id_str);
        }
        catch (...) {
            cout << "Ошибка чтения next_id из файла" << endl;
        }
    }

    int loaded_count = 0;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        vector<string> tokens;

        while (getline(ss, token, '|')) {
            tokens.push_back(token);
        }

        if (tokens.size() == 5) {
            try {
                int id = stoi(tokens[0]);
                string name = tokens[1];
                int workshop_count = stoi(tokens[2]);
                int working_workshop_count = stoi(tokens[3]);
                string station_class = tokens[4];

                // Создаем КС с существующим ID
                CS c;
                c.id = id;
                c.name = name;
                c.workshop_count = workshop_count;
                c.working_workshop_count = working_workshop_count;
                c.station_class = station_class;
                c.created = true;

                css.push_back(c);
                loaded_count++;

            }
            catch (...) {
                cout << "Ошибка разбора строки КС: " << line << endl;
            }
        }
    }

    file.close();
    cout << "КС загружены из файла: " << filename << " (записей: " << loaded_count << ")" << endl;
    logAction("Загрузка КС из файла: " + filename + ", записей: " + to_string(loaded_count));
}

void loadData(vector<Pipe>& pipes, vector<CS>& css) {
    cout << "\n=== Загрузка данных ===\n";

    string pipe_filename = getStringInput("Введите имя файла для труб (по умолчанию pipes.txt): ", true, false);
    if (pipe_filename.empty()) pipe_filename = "pipes.txt";

    string cs_filename = getStringInput("Введите имя файла для КС (по умолчанию cs.txt): ", true, false);
    if (cs_filename.empty()) cs_filename = "cs.txt";

    // Очищаем текущие данные
    pipes.clear();
    css.clear();

    loadPipesFromFile(pipes, pipe_filename);
    loadCSFromFile(css, cs_filename);
}

// ------------------ Меню ------------------
void ShowMenu(vector<Pipe>& pipes, vector<CS>& css) {
    while (true) {
        cout << "\n=== ГЛАВНОЕ МЕНЮ ===\n"
            << "1. Добавить трубу\n"
            << "2. Добавить КС\n"
            << "3. Просмотр всех труб\n"
            << "4. Просмотр всех КС\n"
            << "5. Редактировать трубу\n"
            << "6. Редактировать КС\n"
            << "7. Удалить трубу\n"
            << "8. Удалить КС\n"
            << "9. Поиск труб\n"         
            << "10. Поиск КС\n"     
            << "11. Пакетное редактирование труб\n"
            << "12. Сохранить данные\n"
            << "13. Загрузить данные\n"
            << "0. Выход\n";

        int choice = getIntegerInput("Выберите действие: ", 0, 13);

        switch (choice) {
        case 1:
            addPipe(pipes);
            break;
        case 2:
            addCS(css);
            break;
        case 3:
            showPipes(pipes);
            break;
        case 4:
            showCSs(css);
            break;
        case 5:
            editPipeAt(pipes);
            break;
        case 6:
            editCSAt(css);
            break;
        case 7:
            deletePipeAt(pipes);
            break;
        case 8:
            deleteCSAt(css);
            break;
        case 9:
            searchPipes(pipes);
            break;
        case 10:
            searchCSs(css);
            break;
        case 11:
            batchEditPipes(pipes);
            break;
        case 12:
            saveData(pipes, css);
            break;
        case 13:
            loadData(pipes, css);
            break;
        case 0:
            cout << "Выход из программы. До свидания!\n";
            logAction("Выход из программы");
            return;
        default:
            cout << "Неверный выбор! Пожалуйста, введите число от 0 до 13.\n";
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "Программа для учета труб и компрессорных станций (множественные объекты)\n";
    vector<Pipe> pipes;
    vector<CS> css;
    logAction("Запуск программы");
    ShowMenu(pipes, css);
    return 0;
}
