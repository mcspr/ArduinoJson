// MIT License

#pragma once

#define ARDUINOJSON_EXPECT(X, V) __builtin_expect((X), (V))

#define ARDUINOJSON_LIKELY(X) ARDUINOJSON_EXPECT((X), 1)
#define ARDUINOJSON_UNLIKELY(X) ARDUINOJSON_EXPECT((X), 0)
