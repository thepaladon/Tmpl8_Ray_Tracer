#pragma once
#include "jobsys.h"
namespace Tmpl8
{
    
    class bvh_node : public hittable
    {
    public:
        bvh_node() = default;
        bvh_node(const hittable_list& list, float time0, float time1)
            :bvh_node(list.objects, 0, list.objects.size(), time0, time1) {};

        bvh_node(const std::vector<hittable*>& src_objects, size_t start, size_t end, float time0, float time1);

        bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        bool boundingBox(float t0, float t1, aabb& box) const override;

        void createLeftNode(const std::vector<hittable*>& src_objects,
            size_t start, size_t mid, float time0, float time1) { left = new bvh_node(src_objects, start, mid, time0, time1); } ;

        void createRightNode(const std::vector<hittable*>& src_objects,
            size_t start, size_t mid, float time0, float time1) { right = new bvh_node(src_objects, start, mid, time0, time1); }

        aabb box;
        hittable* left;
        hittable* right;
        
    };

    inline bool boxCompare(const hittable* a, const hittable* b, int axis)
    {
        aabb box_a;
        aabb box_b;

        if (!a->boundingBox(0, 0, box_a) || !b->boundingBox(0, 0, box_b))
            printf("No bounding box in bvh_node constructor \n");

        return box_a.min()[axis] < box_b.min()[axis];
    }

    bool box_x_compare(const hittable* a, const hittable* b) {
        return boxCompare(a, b, 0);
    }

    bool box_y_compare(const hittable* a, const hittable* b) {
        return boxCompare(a, b, 1);
    }

    bool box_z_compare(const hittable* a, const hittable* b) {
        return boxCompare(a, b, 2);
    }


    inline bvh_node::bvh_node(const std::vector<hittable*>& src_objects, size_t start, size_t end, float time0, float time1)
    {
    	std::vector<hittable*> objects = src_objects;

	    const int axis = static_cast<int>(floorf(random_float(0.f, 3.f)));

	    const auto comparator = (axis == 0)
		                            ? box_x_compare
		                            : (axis == 1)
		                            ? box_y_compare
		                            : box_z_compare;

	    const size_t object_span = end - start;

	    if (object_span == 1)
	    {
		    left = right = objects[start];
	    }
	    else if (object_span == 2)
	    {
		    if (comparator(objects[start], objects[start + 1]))
		    {
			    left = objects[start];
			    right = objects[start + 1];
		    }
		    else
		    {
			    left = objects[start + 1];
			    right = objects[start];
		    }
	    }
	    else
	    {
            //median split of the objects based on random axis
		    std::sort(objects.begin() + start, objects.begin() + end, comparator);

		    const size_t mid = start + object_span / 2;

		    left = new bvh_node(objects, start, mid, time0, time1);
		    right = new bvh_node(objects, mid, end, time0, time1);
	    }

	    aabb box_left, box_right;
	    if (!left->boundingBox(time0, time1, box_left) || !right->boundingBox(time0, time1, box_right))
		    printf("No bounding box in bvh_node constructor \n");

	    box = surroundingBox(box_left, box_right);
    }

    inline bool bvh_node::boundingBox(float t0, float t1, aabb& b) const
    {
        b = box;
        return true;
    }

    inline bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {

    	if (!box.hit(r, t_min, t_max))
            return false;

        const bool hit_left = left->hit(r, t_min, t_max, rec);
        const bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

        return hit_left || hit_right;
    }

}
