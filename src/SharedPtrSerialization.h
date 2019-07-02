//
// Created by timluchterhand on 02.07.19.
//

#ifndef SOPRAGAMELOGIC_SHAREDPTRSERIALIZATION_H
#define SOPRAGAMELOGIC_SHAREDPTRSERIALIZATION_H

#include <SopraMessages/json.hpp>

namespace std{

    /**
     * Method for serializing shared_ptrs
     * @tparam T Type of object
     * @param j json instance
     * @param t shared_ptr to be serialized
     */
    template <typename  T>
    void to_json(nlohmann::json &j, const std::shared_ptr<T> &t) {
        if(t) {
            j = *t;
        } else {
            throw std::runtime_error("Cannot serialize nullptr");
        }
    }

    /**
     * Method for deserializing shared_ptrs
     * @tparam T Type of object
     * @param j json instance
     * @param t target
     */
    template <typename T>
    void from_json(const nlohmann::json &j, std::shared_ptr<T> &t) {
        if(j.is_null()) {
            t = nullptr;
        } else {
            t = std::make_shared<T>(j.get<T>());
        }
    }
}


#endif //SOPRAGAMELOGIC_SHAREDPTRSERIALIZATION_H
