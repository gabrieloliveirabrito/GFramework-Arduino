#pragma once

enum ButtonState
{
    Idle = (1u << 0),
    Pressed = (1u << 1),
    Holding = (1u << 2),
    Released = (1u << 3),
};