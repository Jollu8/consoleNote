#include <iostream>
#include <algorithm>
#include <string>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <deque>
#include <string>
#include <time.h>
#include <memory>

struct Event {
    u_int id;
    time_t created; // Дата создания: ДД.ММ.ГГГГ + часы и минуты
    time_t expires; // Дата окончания: ДД.ММ.ГГГГ
    std::string description; // Описание
};


struct Birthday {
    u_int id;
    std::chrono::year_month_day date; // Дата ДД.ММ.ГГГГ
    std::string full_name[3]; // Строки ФИО
    int age; // Возраст
};

struct StructForSortCompare {
    time_t t;
    u_long hash;
};

struct tm *timeTToDate(time_t date) {
    struct tm *t = localtime(&date);
    return t;
}


class Note {
    std::unordered_map<u_long, Event> eventData;
    std::unordered_map<u_long, Birthday> birthdayData;
    std::deque<StructForSortCompare> idEvent, idBirthday;


    u_long hash_str(std::string_view str);

    time_t getCurrentTime() {
        time_t t = time(0);
    }

    time_t dateToTimeT(struct tm *date) {
        return mktime(date);
    }

    u_int count = 0;


public:

    void sort_ev() {
        std::sort(idEvent.begin(), idEvent.end(), [](auto a, auto b) {
            return a.t < b.t;
        });

    };

    void sort_br() {
        std::sort(idBirthday.begin(), idBirthday.end(), [](auto a, auto b) {
            return a.t < b.t;
        });
    }

    u_int get_count_events() {
        return count;
    }

    // Добавляем новое событие
    void add_event(Event &ev);

    // Добавляем новый день рождения
    void add_birthday(Birthday &br);

    bool delete_event(u_long hash);


    Event func_event();

    Birthday func_birthday();

    void print_event();

    void print_birthday();
};


void Note::add_event(Event &ev) {

    StructForSortCompare sfsc;
    sfsc.hash = std::hash<std::string>{}(ev.description);
    if (!eventData.contains(sfsc.hash)) {
        sfsc.t = ev.expires;
        idEvent.push_back(sfsc);
        eventData[sfsc.hash] = std::move(ev);
        count++;
    } else {
        std::cout << "Already has!" << '\n';

    }
}

void Note::add_birthday(Birthday &br) {
    StructForSortCompare sfsc;
    sfsc.hash = std::hash<std::string>{}(br.full_name[0] + br.full_name[1] + br.full_name[2]);
    if (!birthdayData.contains(sfsc.hash)) {

        std::string ans;
        ans = std::to_string(static_cast<u_int>(br.date.day())) + '/';
        ans += std::to_string(static_cast<u_int>(br.date.month())) + '/';
        ans += std::to_string(static_cast<int>(br.date.year()));
        struct tm date = {};
        strptime(ans.c_str(), "%dd/%mm/%yyyy", &date);
        auto t = dateToTimeT(&date);
        sfsc.t = t;
        std::chrono::time_point now{std::chrono::system_clock::now()};
        br.age = static_cast<int >(br.date.year());
        birthdayData[sfsc.hash] = std::move(br);
        count++;
    } else {
        std::cout << "Already has" << 'n';
    }
}

bool Note::delete_event(u_long hash) {
    if (!idEvent.empty()) {
        auto f = eventData.find(hash);
        std::cout << "Event id: " << &f->second.id << "Description: "
                  << &f->second.description << "\n  successfully Deleted!" << '\n';
        eventData.erase(hash);
        idEvent.pop_front();
        count--;
        return true;
    } else return false;
}


void Note::print_event() {
    for (auto &i: idEvent) {
        u_long hash = i.hash;
        auto f = eventData.find(hash);

        std::cout << "[ID: " << f->second.id << "]  "
                  << "[Desc] " << f->second.description << '\n'
                  << "[Created :" << f->second.created << "] - [expired: "
                  << f->second.expires << "]\n\n";
    }
}

void Note::print_birthday() {
    for (auto &it: idBirthday) {
        u_long hash = it.hash;
        auto f = birthdayData.find(hash);
        std::string convertDate;
        // convert Chronos time to string

        convertDate += std::to_string(static_cast<u_int>(f->second.date.day())) + ':';
        convertDate += std::to_string(static_cast<u_int>(f->second.date.month())) + ':';
        convertDate += std::to_string(static_cast<int>(f->second.date.year()));


        std::cout << "[ID: " << f->second.id << "] " << "Birthday: " << convertDate << '\n'
                  << "FullName: " << f->second.full_name[0] << " " << f->second.full_name[1]
                  << " " << f->second.full_name[2] << '\n'
                  << "[Age]" << f->second.age << "\n\n";
    }
}

time_t dateToTimeT(struct tm *date) {
    time_t t = mktime(date);
    return t;
}


Event Note::func_event() {
    Event nEvent;
    nEvent.created = time(nullptr);
    std::cout << "Enter Description: \n>";
    std::cin.ignore();
    getline(std::cin, nEvent.description);
    std::cout << "Enter expiration date (DD.MM.YYYY): ";
    std::string dateStr;
    std::cin.ignore();
    std::getline(std::cin, dateStr);
    struct tm date = {};
    strptime(dateStr.c_str(), "%d/%m/%Y", &date);
    nEvent.expires = dateToTimeT(&date);
    nEvent.id = idEvent.size() + 1;
    return nEvent;
}

Birthday Note::func_birthday() {
    Birthday nBirthday;
    nBirthday.id = idBirthday.size() + 1;
    std::cout << "Enter First name: >";
    std::cin >> nBirthday.full_name[0];
    std::cout << "Enter last name: >";
    std::cin >> nBirthday.full_name[1];
    std::cout << "Enter middle name: >";
    std::cin >> nBirthday.full_name[2];


}


class TimeIter {
    std::chrono::seconds seconds;
    std::chrono::minutes minutes;
    std::chrono::hours hours;
    std::chrono::day day;

public:
    TimeIter(TimeIter&&ti) {
        seconds = std::move(ti.seconds);
        minutes = std::move(ti.minutes);
        hours = std::move(ti.hours);
        day = std::move(ti.day);


    }
    void set_second(const std::chrono::seconds &s);

    void set_minute(const std::chrono::minutes &m);

    void set_hours(const std::chrono::hours);

    void set_day(const std::chrono::day &day);

    const std::chrono::seconds &getSeconds() const;

    const std::chrono::minutes &getMinutes() const;

    const std::chrono::hours &getHours() const;

    const std::chrono::day &getDay() const;



};

int main() {

    Note note;
    bool running = true;
    while (running) {
        char input;
        std::cout << "Choose action (1 - Add event, 2 - Add birthday, 3 - List Events, 4 - List birthday 5 - Exit): ";
        std::cin >> input;
        switch (input) {
            case '1': {
                auto res_ev = note.func_event();
                note.add_event(res_ev);
                break;
            }
            case 2: {
                auto res_birth = note.func_birthday();
                note.add_birthday(res_birth);
                break;
            }
            case 3: {
                note.sort_ev();
                note.print_event();
                break;
            }
            case 4: {
                note.sort_br();
                note.print_birthday();
                break;
            }
            case 5: {
                running = false;
                break;
            }


        }
    }
    std::cout << "Level two\n";
    // chronos for loop
    std::chrono::seconds seconds;
    for (auto i = 0; i < 100; i++) {

    }
}
