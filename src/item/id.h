#pragma once

#include "src/include/namespace.h"

namespace YJS_NAMESPACE{
	typedef unsigned int Clock;
	typedef unsigned int ClientId;

	class Id
		{
		public:

			Id() {};
			~Id() {};

			Id(int client, int clock)
				:client(client), clock(clock){};

			ClientId client = 0;
			Clock clock = 0;

			Id& operator++() {
				++this->clock;
				return *this;
			}

			bool operator==(const Id& id) const{
				return id.client == this->client && id.clock == this->clock;
			}

			Id operator+(const int &offset) const {
				return Id(this->client, this->clock+offset);
			}

			Id& operator+=(int offset) {
				this->clock+=offset;
				return *this;
			}
			


		};

}
