#ifndef EVENT_H
#define EVENT_H

namespace jgl {
    struct Event {
        enum Key {
            UNKNOWN = -1,
            SPACE = 32,
            APOS = 39,
            COMMA = 44,
            MINUS,
            PERIOD,
            SLASH,
            ZERO,
            ONE,
            TWO,
            THREE,
            FOUR,
            FIVE,
            SIX,
            SEVEN,
            EIGHT,
            NINE,
            SEMICOLON = 59,
            EQUAL = 61,
            A = 65,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z,
            BRACKET_OPEN,
            BACKSLASH,
            BRACKET_CLOSE,
            GRAVES = 96,
            ESCAPE = 256,
            ENTER,
            TAB,
            BACKSPACE,
            INSERT_KEY,
            DELETE_KEY,
            ARROW_RIGHT,
            ARROW_LEFT,
            ARROW_DOWN,
            ARROW_UP,
            PAGE_UP,
            PAGE_DOWN,
            HOME,
            END,
            CAPS,
            SCROLL_KEY,
            NUM,
            PRINT,
            PAUSE,
            F1 = 290,
            F2,
            F3,
            F4,
            F5,
            F6,
            F7,
            F8,
            F9,
            F10,
            F11,
            F12,
            F13,
            F14,
            F15,
            F16,
            F17,
            F18,
            F19,
            F20,
            F21,
            F22,
            F23,
            F24,
            F25,
            LEFT_SHIFT = 340,
            LEFT_CONTROL,
            LEFT_ALT,
            LEFT_SUPER,
            RIGHT_SHIFT,
            RIGHT_CONTROL,
            RIGHT_ALT,
            RIGHT_SUPER
        };
        enum Button {
            LEFT,
            RIGHT
        };
        enum Action {
            NONE = -1,
            RELEASE,
            PRESS,
            REPEAT
        };
        enum Mod {
            SHIFT = 0x01,
            CONTROL = 0x02,
            ALT = 0x04,
            SUPER = 0x08
        };
        enum Type {
            KEY,
            MOUSE,
            SCROLL,
            CURSOR,
            RESIZE,
            CLOSE,
            MOVE,
            GAIN_FOCUS,
            LOSE_FOCUS,
            CURSOR_ENTER,
            CURSOR_LEAVE
        };
        double x, y;
        int width, height;
        int code;
        Action action;
        int mods;
        Type type;
        Event(Type, int, Action, int);
        Event(Type, double, double);
        Event(Type, int,int);
        Event(Type);
    };
}

#endif // EVENT_H
