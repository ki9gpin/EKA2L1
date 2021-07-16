/*
 * Copyright (c) 2020 EKA2L1 Team
 * 
 * This file is part of EKA2L1 project
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <common/region.h>
#include <common/algorithm.h>

#include <climits>

namespace eka2l1::common {
    /**
     * NOTE: CODE REVIEWED FROM SYMBIAN OPEN SOURCE.
     */

    eka2l1::rect region::bounding_rect() const {
        eka2l1::vec2 tl { INT_MAX, INT_MAX };
        eka2l1::vec2 br { INT_MIN, INT_MIN };

        for (std::size_t i = 0; i < rects_.size(); i++) {
            tl.x = common::min(tl.x, rects_[i].top.x);
            tl.y = common::min(tl.y, rects_[i].top.y);
            br.x = common::max(br.x, rects_[i].top.x + rects_[i].size.x);
            br.y = common::max(br.y, rects_[i].top.y + rects_[i].size.y);
        }

        return eka2l1::rect { tl, br - tl };
    }

    bool region::add_rect(const eka2l1::rect &rect) {
        if (rect.empty()) {
            return true;
        }

        if (rects_.empty()) {
            rects_.push_back(rect);
            return true;
        }

        if (rect.contains(bounding_rect())) {
            rects_.clear();
            rects_.push_back(rect);
            
            return true;
        }

        for (std::size_t i = 0; i < rects_.size(); i++) {
            if ((rects_[i].top.x + rects_[i].size.x <= rect.top.x) || (rects_[i].top.x >= rect.top.x + rect.size.x) ||
                (rects_[i].top.y + rects_[i].size.y <= rect.top.y) || (rects_[i].top.y >= rect.top.y + rect.size.y))
                continue;

            if ((rects_[i].top.x <= rect.top.x) && (rects_[i].top.y <= rect.top.y) && 
                (rects_[i].top.x + rects_[i].size.x >= rect.top.x + rect.size.x) && 
                (rects_[i].top.y + rects_[i].size.y >= rect.top.y + rect.size.y)) {
                // This rectangle already covers the new rectangle, no modification done
                return false;
            }

            eka2l1::rect intersector;
            intersector = rect.intersect(rects_[i]);

            if (intersector.top.y + intersector.size.y != rect.top.y + rect.size.y)
                rects_.push_back(eka2l1::rect( { rect.top.x, intersector.top.y }, { rect.size.x, rect.size.y + rect.top.y - intersector.top.y }));

            if (intersector.top.y != rect.top.y)
                rects_.push_back(eka2l1::rect( { rect.top.x, rect.top.y }, { rect.size.x, intersector.top.y - rect.top.y }));

			if (intersector.top.x + intersector.size.x != rect.top.x + rect.size.x)
				rects_.push_back(eka2l1::rect({ intersector.top.x + intersector.size.x, intersector.top.y },
                    { rect.top.x + rect.size.x - intersector.top.x - intersector.size.x, intersector.size.y }));

            if (intersector.top.x != rect.top.x)
                rects_.push_back(eka2l1::rect( { rect.top.x, intersector.top.y }, { intersector.top.x - rect.top.x,
                    intersector.size.y}));

            rects_.erase(rects_.begin() + i);
            return true;
        }

        rects_.push_back(rect);
        return true;
    }

    void region::eliminate(const eka2l1::rect &rect) {
        if (rect.empty()) {
            return;
        }

        std::size_t limit = rects_.size();

        for (std::size_t i = 0; i < limit; i++) {
            if ((rect.top == rects_[i].top) && (rect.size == rects_[i].size)) {
                rects_.erase(rects_.begin() + i);
                return;
            }
            
            const eka2l1::rect intersection_reg = rect.intersect(rects_[i]);

            if (!intersection_reg.empty()) {
                const eka2l1::rect original_iterate = rects_[i];
                rects_.erase(rects_.begin() + i);

                const eka2l1::vec2 intersect_reg_br = intersection_reg.bottom_right();
                const eka2l1::vec2 iterate_br = original_iterate.bottom_right();

                if (iterate_br.y != intersect_reg_br.y) {
                    rects_.push_back(eka2l1::rect({ original_iterate.top.x, intersect_reg_br.y }, { iterate_br.x, iterate_br.y }));
                    rects_.back().transform_from_symbian_rectangle();
                }

                if (iterate_br.x != intersect_reg_br.x) {
                    rects_.push_back(eka2l1::rect({ intersect_reg_br.x, intersection_reg.top.y }, { iterate_br.x, intersect_reg_br.y }));
                    rects_.back().transform_from_symbian_rectangle();
                }

                if (intersection_reg.top.x != original_iterate.top.x) {
                    rects_.push_back(eka2l1::rect({ original_iterate.top.x, intersection_reg.top.y }, { intersection_reg.top.x, intersect_reg_br.y }));
                    rects_.back().transform_from_symbian_rectangle();
                }

                if (intersection_reg.top.y != original_iterate.top.y) {
                    rects_.push_back(eka2l1::rect(original_iterate.top, { iterate_br.x, intersection_reg.top.y }));
                    rects_.back().transform_from_symbian_rectangle();
                }

                limit--;
            }
        }
    }

    void region::eliminate(const region &reg) {
        for (std::size_t i = 0; i < reg.rects_.size(); i++) {
            eliminate(reg.rects_[i]);
        }
    }

    region region::intersect(const region &target) const {
        region intersection;

        for (std::size_t i = 0; i < rects_.size(); i++) {
            for (std::size_t j = 0; j < target.rects_.size(); j++) {
                eka2l1::rect the_intersect = target.rects_[j].intersect(rects_[i]);

                if (!the_intersect.empty()) {
                    intersection.rects_.push_back(the_intersect);
                }
            }
        }

        return intersection;
    }
}