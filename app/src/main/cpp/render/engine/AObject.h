//
// Created by dave on 21.05.2026.
//

#pragma once

class AObject {
protected:
    AObject() = default;
    virtual ~AObject() = default;

    // Разрешаем перемещение
    AObject(AObject&&) noexcept = default;
    AObject& operator=(AObject&&) noexcept = default;
public:
    // Запрещаем копирование
    AObject(const AObject&) = delete;
    AObject& operator=(const AObject&) = delete;
};