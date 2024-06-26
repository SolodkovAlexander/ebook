#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdint.h>
#include <string>
#include <vector>

namespace ebook {
    using RequestCount = uint32_t;  // 0 < x <= 1000000

    namespace detail {
        using namespace std::literals;
        using RequestCommand = std::string;
        const RequestCommand READ_COMMAND = "READ"s;
        const RequestCommand CHEER_COMMAND = "CHEER"s;

        using PageNumber = uint16_t; // 0 < x <= 1000
        using UserId = uint32_t; // 0 < x <= 100000
        const UserId USER_COUNT = UserId(100001); // +1 для взаимооднозначного соответствия user_id и индексу вектора
        const PageNumber PAGE_COUNT = PageNumber(1001); // +1 для взаимооднозначного соответствия page_number и индексу вектора
    }

    // Обрабатывает запросы системы стимулирования чтения электронных книг
    void HandleReadingIncentiveSystemRequests(RequestCount request_count, 
                                              std::istream& request_in, 
                                              std::ostream& request_result_out) {
        using namespace detail;

        // Настраиваем поток для вывода результатов запросов
        const auto default_precision{ request_result_out.precision() };
        request_result_out << std::setprecision(6);

        // Информация о состоянии системы
        std::vector<PageNumber> user_to_page(USER_COUNT);
        std::vector<UserId> page_to_user_count(PAGE_COUNT);
        UserId user_count{0};

        // Параметры запроса
        RequestCommand request_command;
        UserId user_id{0};
        PageNumber page_number{0};

        // Обрабатываем запросы
        while (request_count--) {
            request_in >> request_command >> user_id;
            if (request_command == READ_COMMAND) {
                request_in >> page_number;
                if (!user_to_page.at(user_id)) {
                    ++user_count;
                } else {
                    --page_to_user_count[user_to_page.at(user_id)];
                }
                user_to_page[user_id] = page_number;
                ++page_to_user_count[page_number];
            } else if (request_command == CHEER_COMMAND) {
                double slow_readers_proportion{0.0};
                if (user_to_page.at(user_id)) {
                    if (user_count == 1) {
                        slow_readers_proportion = 1.0;
                    } else {
                        PageNumber user_page = user_to_page.at(user_id);
                        UserId slow_readers_count = std::accumulate(page_to_user_count.begin(),
                                                                    page_to_user_count.begin() + user_page,
                                                                    UserId(0),
                                                                    [](UserId result, const UserId& user_count) { return result + user_count; });
                        slow_readers_proportion = static_cast<double>(slow_readers_count) / static_cast<double>(user_count - 1);
                    }
                }
                request_result_out << slow_readers_proportion << '\n';
            }
        }

        request_result_out << std::setprecision(default_precision);
    }
}

int main() {
    ebook::RequestCount request_count{0};
    std::cin >> request_count;
    ebook::HandleReadingIncentiveSystemRequests(request_count, 
                                                std::cin, 
                                                std::cout);
    return 0;
}