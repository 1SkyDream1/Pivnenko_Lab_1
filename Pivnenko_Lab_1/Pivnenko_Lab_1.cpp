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
    string name;
    double length = 0.0;
    int diameter = 0;
    bool under_repair = false;
    bool created = false; // флаг - труба создана
};

struct CS {
    string name;
    int workshop_count = 0;
    int working_workshop_count = 0;
    string station_class;
    bool created = false; // флаг - КС создана
};

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

// безопасное чтение целого числа
int getIntegerInput(const string& prompt, int min_val = 1, int max_val = numeric_limits<int>::max()) {
    int value;
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        input = trim(input);

        if (input.empty()) {
            cout << "Ошибка! Ввод не может быть пустым. Попробуйте снова.\n";
            continue;
        }

        bool has_non_digit = false;
        for (size_t i = 0; i < input.size(); i++) {
            if (!isdigit(static_cast<unsigned char>(input[i]))) {
                has_non_digit = true;
                break;
            }
        }

        if (has_non_digit) {
            cout << "Ошибка! Введите целое число. Попробуйте снова.\n";
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

    string msg = "Добавлена труба: " + p.name + " (длина " + to_string(p.length) + " км, диаметр " + to_string(p.diameter) + " мм)";
    cout << "Труба '" << p.name << "' успешно добавлена!\n";
    logAction(msg);
}

void showPipes(const vector<Pipe>& pipes) {
    cout << "\n=== Список труб ===\n";
    if (pipes.empty()) {
        cout << "Труб нет.\n";
        return;
    }
    for (size_t i = 0; i < pipes.size(); ++i) {
        const Pipe& p = pipes[i];
        cout << i + 1 << ". " << p.name
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
    string msg = "Добавлена КС: " + cs.name + " (цехов " + to_string(cs.workshop_count) + ", работает " + to_string(cs.working_workshop_count) + ")";
    cout << "КС '" << cs.name << "' успешно добавлена!\n";
    logAction(msg);
}

void showCSs(const vector<CS>& css) {
    cout << "\n=== Список КС ===\n";
    if (css.empty()) {
        cout << "КС нет.\n";
        return;
    }
    for (size_t i = 0; i < css.size(); ++i) {
        const CS& c = css[i];
        cout << i + 1 << ". " << c.name
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
            << "0. Выход\n";

        int choice = getIntegerInput("Выберите действие: ", 0, 8);

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
        case 0:
            cout << "Выход из программы. До свидания!\n";
            logAction("Выход из программы");
            return;
        default:
            cout << "Неверный выбор! Пожалуйста, введите число от 0 до 8.\n";
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
