#include "common.h"
#include "binding.h"
#include "nlohmann/json.hpp"

#define CHECK_TOP(L, TOP)\
auto n = lua_gettop(L);\
if (n < TOP)\
    return 0;

namespace engine
{
    namespace scripting
    {
        namespace json
        {
            void parse(lua_State*L, const nlohmann::json& json)
            {
                std::function<void(const nlohmann::json&)> parse_array = [=](const nlohmann::json& json)
                {
                    lua_newtable(L);
                    
                    int i = 1;
                    
                    int top = lua_gettop(L);
                    
                    for (auto it = json.begin(); it != json.end(); ++it, ++i)
                    {
                        lua_pushnumber(L, i);
                        
                        parse(L, it.value());
                        
                        lua_settable(L, top);
                    }
                };
                
                std::function<void(const nlohmann::json&)> parse_object = [=](const nlohmann::json& json)
                {
                    lua_newtable(L);
                    
                    int top = lua_gettop(L);
                    
                    for (auto it = json.begin(); it != json.end(); ++it)
                    {
                        auto key = it.key();
                
                        lua_pushstring(L, key.c_str());
                        
                        parse(L, it.value());
                        
                        lua_settable(L, top);
                    }
                };
                
                if (json.is_boolean())
                    lua_pushboolean(L, json.get<bool>());
                
                if (json.is_number())
                    lua_pushnumber(L, json.get<double>());
                
                if (json.is_string())
                    lua_pushstring(L, json.get<std::string>().c_str());
                
                if (json.is_array())
                    parse_array(json);
                
                if (json.is_object())
                    parse_object(json);
            }
            
            int parse(lua_State* L)
            {
                CHECK_TOP(L, 1);
                
                try
                {
                    if (lua_isstring(L, 1))
                    {
                        auto data = lua_tostring(L, 1);
                        
                        logger() << "[scripting] parse json:" << data;
                        
                        CLEAR_TOP(L);
                        
                        auto json = nlohmann::json::parse(data);
                        
                        parse(L, json);
                        
                        return 1;
                    }
                }
                catch (const nlohmann::json::parse_error& e)
                {
                    logger() << "[scripting] " << e.what();
                }
                catch (const nlohmann::json::type_error& e)
                {
                    logger() << "[scripting] " << e.what();
                }
                
                return 0;
            }
        }
        
        namespace functions
        {
            int load_script(lua_State* L)
            {
                if (lua_isstring(L, 1))
                {
                    auto script = lua_tostring(L, 1);
                    std::vector<char> data;
                    
                    logger() << "[scripting] load script:" << script;
                    
                    CLEAR_TOP(L)
                    
                    auto path = resources_manager::instance().get_path_to_resource(script);
                    
                    if (file_utils::read_file(path, &data))
                    {
                        if (scripting::load_script(L, data.data(), data.size(), script))
                            return 1;
                    }
                    
                    logger() << "[scripting] error loading file:" << script;
                }
                
                return 0;
            }
            
            int debug_log(lua_State* L)
            {
                std::vector<std::string> strings;
                
                while (lua_gettop(L))
                {
                    if (lua_isstring(L, -1))
                        strings.push_back(lua_tostring(L, -1));
                    
                    if (lua_isnil(L, -1))
                        strings.push_back("nill");
                    
                    if (lua_isboolean(L, -1))
                        strings.push_back(lua_toboolean(L, -1) > 0 ? "true" : "false");
                    
                    lua_pop(L, 1);
                }
                
                for (auto it = strings.rbegin(); it != strings.rend(); ++it)
                    logger() << "[lua] " << *it;
                
                return 0;
            }
            
            int read_file(lua_State* L)
            {
                if (lua_isstring(L, 1))
                {
                    auto file = lua_tostring(L, 1);
                    std::vector<char> data;
                    
                    logger() << "[scripting] read file:" << file;
                    
                    CLEAR_TOP(L)
                    
                    auto path = resources_manager::instance().get_path_to_resource(file);
                    
                    if (file_utils::read_file(path, &data))
                    {
                        lua_pushstring(L, std::string(data.data(), data.data() + data.size()).c_str());
                        return 1;
                    }
                    
                    logger() << "[scripting] error loading file:" << file;
                }
                
                return 0;
            }
        }

		namespace vector3d
		{
            void push(lua_State* L, float x, float y, float z)
            {
                lua_newtable(L);
                lua_pushnumber(L, x);
                lua_setfield(L, -2, "x");
                lua_pushnumber(L, y);
                lua_setfield(L, -2, "y");
                lua_pushnumber(L, z);
                lua_setfield(L, -2, "z");
            }
            
			int create(lua_State* L, float x, float y, float z)
			{
                push(L, x, y, z);

				return 1;
			}

			math::vector3d get(lua_State* L, int n)
			{
                float numbers[] = { 0, 0, 0 };
                
				if (lua_istable(L, n))
				{
                    lua_getfield(L, n, "x");
                    numbers[0] = get_number(L, -1);
                    
                    lua_pop(L, 1);
                    
                    lua_getfield(L, n, "y");
                    numbers[1] = get_number(L, -1);
                    
                    lua_pop(L, 1);
                    
                    lua_getfield(L, n, "z");
                    numbers[2] = get_number(L, -1);
                    
                    lua_pop(L, 1);
				}
                else
                {
                    numbers[0] = get_number(L, n);
                    numbers[1] = get_number(L, n + 1);
                    numbers[2] = get_number(L, n + 2);
                }

				return math::vector3d(numbers[0], numbers[1], numbers[2]);
			}
		}

		namespace color
		{
			int create(lua_State* L, float r, float g, float b)
			{
				lua_newtable(L);
				lua_pushnumber(L, r);
				lua_setfield(L, -2, "r");
				lua_pushnumber(L, g);
				lua_setfield(L, -2, "g");
				lua_pushnumber(L, b);
				lua_setfield(L, -2, "b");

				return 1;
			}
            
            math::vector3d get(lua_State* L, int n)
            {
                float numbers[] = { 0, 0, 0 };
                
                if (lua_istable(L, n))
                {
                    lua_getfield(L, n, "r");
                    numbers[0] = get_number(L, -1);
                    
                    lua_pop(L, 1);
                    
                    lua_getfield(L, n, "g");
                    numbers[1] = get_number(L, -1);
                    
                    lua_pop(L, 1);
                    
                    lua_getfield(L, n, "b");
                    numbers[2] = get_number(L, -1);
                    
                    lua_pop(L, 1);
                }
                else
                {
                    numbers[0] = get_number(L, n);
                    numbers[1] = get_number(L, n + 1);
                    numbers[2] = get_number(L, n + 2);
                }
                
                return math::vector3d(numbers[0], numbers[1], numbers[2]);
            }
		}
        
        namespace game
        {
            int get_mouse_location(lua_State* L)
            {
				auto location = application::instance().get_mouse_location();
                
                vector3d::push(L, location.x, location.y, 0);

                return 1;
            }
            
            int get_world_mouse_location(lua_State* L)
            {
                auto location = application::instance().get_mouse_location();
                auto world = director::instance().convert_screen_to_world(location);
                
                vector3d::push(L, world.x, world.y, world.z);
                
                return 1;
            }
            
            int convert_to_world_space(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto location = vector3d::get(L, 2);
                auto world = director::instance().convert_screen_to_world(math::vector2d(location.x, location.y));
                
                CLEAR_TOP(L);
                
                vector3d::push(L, world.x, world.y, world.z);
                
                return 1;
            }
            
            int get_win_size(lua_State* L)
            {
                auto location = application::instance().get_win_size();
                
                vector3d::push(L, location.x, location.y, 0);
                
                return 1;
            }
            
            int get_delta_time(lua_State* L)
            {
                auto time = director::instance().get_delta_time();
                
                lua_pushnumber(L, time);
                
                return 1;
            }
            
            int get_local_time(lua_State* L)
            {
                auto time = director::instance().get_local_time();
                
                lua_pushinteger(L, time);
                
                return 1;
            }
        }

        namespace game_object
        {
            template<class T, class ...Args>
            int create(lua_State* L, Args... args)
            {
                auto obj = engine::game_object::create<T>(args...);
                
                if (!obj)
                    return 0;
                
                push_ref(L, obj);
                return 1;
            }
            
            int create(lua_State* L)
            {
                return create<engine::game_object>(L);
            }
            
            int destroy(lua_State* L)
            {
                return scripting::destroy_ref<engine::game_object>(L);
            }
            
            int add_child(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                auto child = scripting::get<engine::game_object>(L, 2);
                
                if (obj && child)
                    obj->add_child(child);
                
                CLEAR_TOP(L);
                
                return 0;
            }
            
            int remove_child(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                auto child = scripting::get<engine::game_object>(L, 2);
                
                if (obj && child)
                    obj->remove_child(child);
                
                CLEAR_TOP(L);
                
                return 0;
            }
            
            int remove_from_parent(lua_State* L)
            {
                CHECK_TOP(L, 1);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                
                if (obj)
                    obj->remove_from_parent();
                
                CLEAR_TOP(L);
                
                return 0;
            }
            
            int add_component(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                auto component = scripting::get<engine::component>(L, 2);
                
                if (obj && component)
                    obj->add_component(component);
                
                CLEAR_TOP(L);
                
                return 0;
            }
            
            int remove_component(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                auto component = scripting::get<engine::component>(L, 1);
                
                if (obj && component)
                    obj->remove_component(component);
                
                CLEAR_TOP(L);
                
                return 0;
            }

			int set_enabled(lua_State* L)
			{
				CHECK_TOP(L, 2);

				if (!lua_isboolean(L, 2))
					return 0;

				auto obj = scripting::get<engine::game_object>(L, 1);

				if (obj)
					obj->set_enabled(lua_toboolean(L, 2) > 0);
                
                CLEAR_TOP(L);

				return 0;
			}

			int get_enabled(lua_State* L)
			{
				CHECK_TOP(L, 1);

				auto obj = scripting::get<engine::game_object>(L, 1);

				if (obj)
				{
                    CLEAR_TOP(L);
                    
					lua_pushboolean(L, obj->get_enabled());
					return 1;
				}

				return 1;
			}

			int set_position(lua_State* L)
			{
				CHECK_TOP(L, 1);

				auto obj = scripting::get<engine::game_object>(L, 1);

				if (!obj)
					return 0;
                
				obj->set_position(vector3d::get(L, 2));
                
                CLEAR_TOP(L);

				return 0;
			}

			int get_position(lua_State* L)
			{
				CHECK_TOP(L, 1);

				auto obj = scripting::get<engine::game_object>(L, 1);

				if (!obj)
					return 0;

				auto position = obj->get_position();
                
                CLEAR_TOP(L);

				return vector3d::create(L, position.x, position.y, position.z);
			}

			int set_rotation(lua_State* L)
			{
				CHECK_TOP(L, 1);

				auto obj = scripting::get<engine::game_object>(L, 1);

				if (!obj)
					return 0;

				obj->set_rotation(vector3d::get(L, 2));
                
                CLEAR_TOP(L);

				return 0;
			}

			int get_rotation(lua_State* L)
			{
                CHECK_TOP(L, 1);
                
				auto obj = scripting::get<engine::game_object>(L, 1);

				if (!obj)
					return 0;

				auto rotation = obj->get_rotation();
                
                CLEAR_TOP(L);

				return vector3d::create(L, rotation.x, rotation.y, rotation.z);
			}

			int set_scale(lua_State* L)
			{
				CHECK_TOP(L, 1);

				auto obj = scripting::get<engine::game_object>(L, 1);

				if (!obj)
					return 0;

				obj->set_scale(vector3d::get(L, 2));
                
                CLEAR_TOP(L);

				return 0;
			}

			int get_scale(lua_State* L)
			{
                CHECK_TOP(L, 1);
                
				auto obj = scripting::get<engine::game_object>(L, 1);

				if (!obj)
					return 0;

				auto scale = obj->get_scale();
                
                CLEAR_TOP(L);

				return vector3d::create(L, scale.x, scale.y, scale.z);
			}

			int set_size(lua_State* L)
			{
				CHECK_TOP(L, 1);

				auto obj = scripting::get<engine::game_object>(L, 1);

				if (!obj)
					return 0;

				obj->set_size(vector3d::get(L, 2));
                
                CLEAR_TOP(L);

				return 0;
			}

			int get_size(lua_State* L)
			{
                CHECK_TOP(L, 1);
                
				auto obj = scripting::get<engine::game_object>(L, 1);

				if (!obj)
					return 0;

				auto size = obj->get_size();
                
                CLEAR_TOP(L);

				return vector3d::create(L, size.x, size.y, size.z);
			}

			int set_anchor(lua_State* L)
			{
				CHECK_TOP(L, 1);

				auto obj = scripting::get<engine::game_object>(L, 1);

				if (!obj)
					return 0;

				obj->set_anchor(vector3d::get(L, 2));
                
                CLEAR_TOP(L);

				return 0;
			}

			int get_anchor(lua_State* L)
			{
                CHECK_TOP(L, 1);
                
				auto obj = scripting::get<engine::game_object>(L, 1);

				if (!obj)
					return 0;

				auto anchor = obj->get_anchor();
                
                CLEAR_TOP(L);

				return vector3d::create(L, anchor.x, anchor.y, anchor.z);
			}
            
            int set_tag(lua_State* L)
            {
                CHECK_TOP(L, 1);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                
                if (!obj)
                    return 0;
                
                int tag = (int)lua_tointeger(L, 2);
                
                obj->set_tag(tag);
                
                CLEAR_TOP(L);
                
                return 0;
            }
            
            int get_tag(lua_State* L)
            {
                CHECK_TOP(L, 1);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                
                if (!obj)
                    return 0;
                
                CLEAR_TOP(L);
                
                lua_pushnumber(L, obj->get_tag());
                
                return 1;
            }
            
            int set_opacity(lua_State* L)
            {
                CHECK_TOP(L, 1);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                
                if (!obj)
                    return 0;
                
                int opacity = get_integer(L, 2);
                
                obj->set_opacity(opacity);
                
                CLEAR_TOP(L);
                
                return 0;
            }
            
            int get_opacity(lua_State* L)
            {
                CHECK_TOP(L, 1);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                
                if (!obj)
                    return 0;
                
                CLEAR_TOP(L);
                
                lua_pushinteger(L, obj->get_opacity());
                
                return 1;
            }
            
            int get_children_count(lua_State* L)
            {
                CHECK_TOP(L, 1);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                
                if (!obj)
                    return 0;
                
                CLEAR_TOP(L);
                
                lua_pushnumber(L, obj->get_children_count());
                
                return 1;
            }
            
            int get_parent(lua_State* L)
            {
                CHECK_TOP(L, 1);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                
                if (!obj)
                    return 0;
                
                CLEAR_TOP(L);
                
                push_ref<engine::game_object>(L, obj->get_parent());
                
                return 1;
            }
            
            int local_to_global(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                
                if (!obj)
                    return 0;
                
                auto location = obj->local_to_global(vector3d::get(L, 2));
                
                CLEAR_TOP(L);

                vector3d::push(L, location.x, location.y, location.z);
                
                return 1;
            }
            
            int global_to_local(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto obj = scripting::get<engine::game_object>(L, 1);
                
                if (!obj)
                    return 0;
                
                auto location = obj->global_to_local(vector3d::get(L, 2));
                
                CLEAR_TOP(L);
                
                vector3d::push(L, location.x, location.y, location.z);
                
                return 1;
            }
        }
        
        namespace sprite
        {
            int create(lua_State* L)
            {
                if (lua_isstring(L, 1))
                {
					if (lua_isstring(L, 2))
					{
						auto atlas = lua_tostring(L, 1);
						auto texture = lua_tostring(L, 2);

						CLEAR_TOP(L);

						return game_object::create<engine::sprite>(L, atlas, texture);
					}

                    auto texture = lua_tostring(L, 1);
                    
                    CLEAR_TOP(L);
                    
                    return game_object::create<engine::sprite>(L, texture);
                } 
                
                return game_object::create<engine::sprite>(L);
            }
            
            int destroy(lua_State* L)
            {
                return scripting::destroy_ref<engine::sprite>(L);
            }
            
            int set_color(lua_State* L)
            {
                CHECK_TOP(L, 1);

                auto obj = scripting::get<engine::sprite>(L, 1);
                
                if (!obj)
                    return 0;
                
                obj->set_color(color::get(L, 2));
                
                CLEAR_TOP(L);
                
                return 0;
            }

            int get_color(lua_State* L)
            {
                CHECK_TOP(L, 1);

                auto obj = scripting::get<engine::sprite>(L, 1);
                
                if (!obj)
                    return 0;
                
                auto color = obj->get_color();
                
                CLEAR_TOP(L);
                
                return color::create(L, color.x, color.y, color.z);
            }

            int set_texture(lua_State* L)
            {
                CHECK_TOP(L, 1);

                auto obj = scripting::get<engine::sprite>(L, 1);
                
                if (!obj || !lua_isstring(L, 2))
                    return 0;
                
                auto texture = lua_tostring(L, 2);
                obj->set_texture(texture);
                
                CLEAR_TOP(L);
                
                return 0;
            }
            
            int set_alpha(lua_State* L)
            {
                CHECK_TOP(L, 1);
                
                auto obj = scripting::get<engine::sprite>(L, 1);
                
                if (!obj)
                    return 0;
                
                auto alpha = get_number(L, 2);
                obj->set_alpha(alpha);
                
                CLEAR_TOP(L);
                
                return 0;
            }
            
            int get_alpha(lua_State* L)
            {
                CHECK_TOP(L, 1);
                
                auto obj = scripting::get<engine::sprite>(L, 1);
                
                if (!obj)
                    return 0;
                
                CLEAR_TOP(L);
                
                auto alpha = obj->get_alpha();
                
                lua_pushnumber(L, alpha);
                
                return 1;
            }
        }
        
        namespace scene
        {
            int create(lua_State* L)
            {
                return game_object::create<engine::game_object>(L);
            }
        }
        
        namespace box_collider2d
        {
            int create(lua_State* L)
            {
                auto w = get_number(L, 1);
                auto h = get_number(L, 2);
                
                CLEAR_TOP(L);
                
                auto collider = ref::create<engine::box_collider2d>();
                
                collider->set_size(math::vector2d(w, h));
                push_ref(L, collider);
                
                return 1;
            }
            
            int destroy(lua_State* L)
            {
                return destroy_ref<engine::box_collider2d>(L);
            }
            
            int on_click(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto collider = get<engine::box_collider2d>(L, 1);
                auto mouse = vector3d::get(L, 2);
                
                CLEAR_TOP(L);
                
                if (collider)
                {
                    auto result = collider->on_click(math::vector2d(mouse.x, mouse.y));
                    lua_pushboolean(L, result);
                }
                
                return 1;
            }
        }
        
        namespace targeted_action
        {
            int create(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto target = get<engine::game_object>(L, 1);
                auto action = get<engine::action>(L, 2);
                
                if (target && action)
                {
                    auto targeted = action::create<engine::targeted_action>(action, target);
                    
                    CLEAR_TOP(L);
                    
                    push_ref(L, targeted);
                }
                
                return 1;
            }
            
            int destroy(lua_State* L)
            {
                return destroy_ref<engine::targeted_action>(L);
            }
        }
        
        namespace action_lua_callback
        {
            int create(lua_State* L)
            {
                if (!lua_isfunction(L,1))
                    luaL_argerror(L, 1, "expected function");
                
				auto scripts = resources_manager::instance().get_resources<engine::script>();

				for (auto script : scripts)
				{
					if (script->get_state() == L)
					{
						lua_pushvalue(L, -1);
						auto handler = luaL_ref(L, LUA_REGISTRYINDEX);
						auto action = action::create<engine::action_lua_callback>(script, handler);

						push_ref(L, action);

						return 1;
					}
				}
                
                return 0;
            }
            
            int destroy(lua_State* L)
            {
                return destroy_ref<engine::action_lua_callback>(L);
            }
        }
        
        namespace action_sequence
        {
            int create(lua_State* L)
            {
                vector<engine::action*> actions;
                
                while (lua_gettop(L))
                {
                    auto action = get<engine::action>(L, -1);
                    
                    lua_pop(L, 1);
                    
                    actions.push_back(action);
                }
                
                auto sequence = action::create<engine::action_sequence>();
                
                for (auto it = actions.rbegin(); it != actions.rend(); ++it)
                    sequence->append(*it);
                
                push_ref(L, sequence);
                
                return 1;
            }
            
            int append(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto sequence = get<engine::action_sequence>(L, 1);
                auto action = get<engine::action>(L, 2);
                
                CLEAR_TOP(L);
                
                if (sequence && action)
                    sequence->append(action);
                
                return 0;
            }
            
            int destroy(lua_State* L)
            {
                return destroy_ref<engine::action_sequence>(L);
            }
        }
        
        namespace action_list
        {
            int create(lua_State* L)
            {
                vector<engine::action*> actions;
                
                while (lua_gettop(L))
                {
                    auto action = get<engine::action>(L, -1);
                    
                    lua_pop(L, 1);
                    
                    actions.push_back(action);
                }
                
                auto list = action::create<engine::action_list>();
                
                push_ref(L, list);
                
                return 1;
            }
            
            int append(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto list = get<engine::action_list>(L, 1);
                auto action = get<engine::action>(L, 2);
                
                CLEAR_TOP(L);
                
                if (list && action)
                    list->append(action);
                
                return 0;
            }
            
            int destroy(lua_State* L)
            {
                return destroy_ref<engine::action_list>(L);
            }
        }
        
        namespace action_delay
        {
            int create(lua_State* L)
            {
                auto duration = get_number(L, 1);
                auto action = action::create<engine::action_delay>(duration);
                
                CLEAR_TOP(L);
                
                push_ref(L, action);
                
                return 1;
            }
            
            int destroy(lua_State* L)
            {
                return destroy_ref<engine::action_delay>(L);
            }
        }
        
        namespace action_move
        {
            int create(lua_State* L)
            {
                CHECK_TOP(L, 2);
                
                auto from = vector3d::get(L, 1);
                auto to = vector3d::get(L, 2);
                auto duration = get_number(L, 3);
                
                CLEAR_TOP(L);
                
                auto action = engine::action_move::move(from, to, duration);
                
                push_ref(L, action);
                
                return 1;
            }
            
            int move_to(lua_State* L)
            {
                auto to = vector3d::get(L, 1);
                auto duration = get_number(L, 2);
                
                CLEAR_TOP(L);
                
                auto action = engine::action_move::move_to(to, duration);
                
                push_ref(L, action);
                
                return 1;
            }
            
            int move_by(lua_State* L)
            {
                auto by = vector3d::get(L, 1);
                auto duration = get_number(L, 2);
                
                CLEAR_TOP(L);
                
                auto action = engine::action_move::move_by(by, duration);
                
                push_ref(L, action);
                
                return 1;
            }
            
            int destroy(lua_State* L)
            {
                return destroy_ref<engine::action_move>(L);
            }
        }
    }
}
