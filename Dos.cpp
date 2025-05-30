#include <iostream>
#include <thread>
#include <vector>
#include <curl/curl.h>
#include <mutex>
#include <chrono>
using namespace std;
std::mutex print_mutex;

void send_request(int i, const std::string& url) {
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

        CURLcode res = curl_easy_perform(curl);

        std::lock_guard<std::mutex> lock(print_mutex);
        if (res == CURLE_OK) {
            std::cout << "Richiesta " << i << ": OK\n";
        } else {
            std::cout << "Richiesta " << i << ": Errore - " << curl_easy_strerror(res) << "\n";
        }
        curl_easy_cleanup(curl);
    }
}

int main() {
    std::string url = "";
    int total_requests = 10 ;// Total number of requests to send
    int max_threads = 1; // Maximum number of concurrent threads depends on system resources

    cout<<"inserisci il numero di richieste da inviare: ";
    std::cin >> total_requests;
    cout<<"inserisci il numero di thread da utilizzare: ";
    std::cin >> max_threads;
    cout<< "URL da utilizzare: ";
    std::cin >> url;
    std::vector<std::thread> threads;
    int i = 0;

    while (i < total_requests) {
        for (int t = 0; t < max_threads && i < total_requests; ++t, ++i) {
            threads.emplace_back(send_request, i, url);
        }

        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }

        threads.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Optional delay to avoid overwhelming the server
    }

    return 0;
}

