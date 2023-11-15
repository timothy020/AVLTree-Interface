#pragma once

#define YJS_NAMESPACE yjs_1209

#define YJS_DEBUG(format, ...) \
fprintf(stderr, "[DEBUG] %s, %s(), %d: " format "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
