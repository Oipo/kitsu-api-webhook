/*
    kitsu_api
    Copyright (C) 2019 Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <iostream>
#include <spdlog/spdlog.h>
#include <optional>
#include <restinio/traits.hpp>
#include <restinio/http_server_run.hpp>
#include <restinio/router/express.hpp>
#include <cpr/cpr.h>

#include "config.h"
#include "logger_init.h"
#include "config_parsers.h"
#include "on_leaving_scope.h"
#include "messages/get_pyra_request_message.h"
#include "messages/pyra_response_message.h"
#include "messages/kitsu_response_message.h"

using namespace std;
using namespace kitsu;
using namespace restinio;

template <typename RESP>
RESP init_resp(RESP resp)
{
    resp.append_header(restinio::http_field::content_type, "application/json");

    return resp;
}

router::express_request_handler_t catch_and_log_any_exception(string name, router::express_request_handler_t const &handler) {
    if(handler == nullptr) {
        throw runtime_error("handler not initialized");
    }

    return [=, &name](request_handle_t req, auto params) {
        auto scope_guard = on_leaving_scope([&]{
            spdlog::debug("done executing handler {}", name);
        });

        try {
            spdlog::debug("executing handler {}", name);
            return handler(req, move(params));
        } catch (exception const &e) {
            spdlog::error("uncaught exception in handler {}: {}", name, e.what());
            return req->create_response(http_status_line_t(status_code::internal_server_error, ""))
                    .connection_close()
                    .done();
        }
    };
}

int main() {
    config config;
    try {
        auto config_opt = parse_env_file();
        if(!config_opt) {
            return 1;
        }
        config = config_opt.value();
    } catch (const exception& e) {
        spdlog::error("[main] config.json file is malformed json.");
        return 1;
    }

    reconfigure_logger(config);

    auto router = make_unique<restinio::router::express_router_t<>>();
    uint16_t threads = config.threads;

    if(threads == 0) {
        threads = thread::hardware_concurrency();
    }

    router->http_post(R"(/find)", catch_and_log_any_exception("/find", [&](request_handle_t req, auto params) {
        unique_ptr<get_pyra_request_message> msg = get_pyra_request_message::deserialize(req->body());
        if(msg == nullptr) {
            spdlog::error("[/find] msg null");
            return req->create_response(http_status_line_t(status_code::not_found, "Not found"))
                    .connection_close()
                    .done();
        }

        pyra_response_message resp_msg{{response {"unknown command", ""}}};

        spdlog::debug("[/find] got command {} and arguments {} from {} in channel {} in guild {}",
                msg->command, msg->arguments, msg->author.name, msg->channel.name, msg->guild.name);
        if(msg->command == "anime") {
            cpr::Session session;
            session.SetVerifySsl({false});
            session.SetUrl(cpr::Url{"https://kitsu.io/api/edge/anime"});
            session.SetHeader(cpr::Header{{"content-type", "application/vnd.api+json"}, {"accept", "application/vnd.api+json"}});
            session.SetParameters(cpr::Parameters{{"filter[text]", msg->arguments}});
            auto r = session.Get();

            spdlog::debug("status_code {} with text size {} in {} seconds", r.status_code, r.text.size(), r.elapsed);
            spdlog::trace("text {}", r.text);

            if(r.error) {
                spdlog::debug("cpr error {}: \"{}\"", (int)r.error.code, r.error.message);
            }

            if(r.status_code != 200) {
                resp_msg.responses = {response {"Kitsu probably down. Check error logs.", ""}};
                return init_resp(req->create_response())
                        .set_body(resp_msg.serialize())
                        .done();
            }

            auto kitsu_msg = kitsu_response_message::deserialize(r.text);
            if(kitsu_msg == nullptr) {
                resp_msg.responses = {response {"Deserialization error. Check error logs.", ""}};
                return init_resp(req->create_response())
                        .set_body(resp_msg.serialize())
                        .done();
            }

            vector<response> responses;

            for (auto &animuh : kitsu_msg->animes) {
                response rsp;
                rsp.image_url = animuh.image_url;
                spdlog::debug("found anime \"[{}/{}] rating {}, ran from {} to {} with {} episodes of {} minutes length. Url: https://kitsu.io/anime/{}\"",
                              animuh.en_title, animuh.jp_title, animuh.avg_rating, animuh.start_date, animuh.end_date, animuh.episodes,
                              animuh.episode_length, animuh.slug);
                rsp.message = fmt::format("[{}/{}] rating {}, ran from {} to {} with {} episodes of {} minutes length. Url: https://kitsu.io/anime/{}",
                                          animuh.en_title, animuh.jp_title, animuh.avg_rating, animuh.start_date, animuh.end_date, animuh.episodes,
                                          animuh.episode_length, animuh.slug);

                if(responses.size() < 1) {
                    responses.emplace_back(move(rsp));
                }
            }

            resp_msg.responses = responses;
        }

        spdlog::debug("Returning {} entries", resp_msg.responses.size());

        return init_resp(req->create_response())
                .set_body(resp_msg.serialize())
                .done();
    }));

    router->non_matched_request_handler(
            [](request_handle_t req){
                spdlog::debug("[{}] wrong url", req->header().request_target());
                return req->create_response( http_status_line_t(status_code::not_found, "Not found"))
                        .connection_close()
                        .done();
            });

    struct my_server_traits : public default_traits_t {
        using request_handler_t = restinio::router::express_router_t<>;
    };

    spdlog::info("[main] running restinio on \"{}:{}\" on {} threads", config.address, config.port, threads);

    try {
        // restinio installs a sigint handler
        restinio::run(
                restinio::on_thread_pool<my_server_traits>(threads)
                        .port(config.port)
                        .address(config.address)
                        .request_handler(move(router)));
    } catch(exception const &e) {
        spdlog::error("[main] uncaught exception: {}", e.what());
    }

    return 0;
}