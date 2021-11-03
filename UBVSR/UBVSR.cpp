#include "UBVSR.hpp"

#include "Buffer.hpp"
#include "FpsCounter.hpp"

#include <future>

void zzz()
{
    //ubv::StencilBuffer color_buffer(640, 480);
    //color_buffer.at(40, 50) = true;
   // const auto &bb = color_buffer;
    //auto &z = bb.at(40, 50);
    //color_buffer.clear();
    // ubv::DepthBuffer depth_buffer(ubv::u16vec2{640,480});
    // ubv::StencilBuffer stencil_buffer(ubv::u16vec2{640,480});
    // stencil_buffer.at(ubv::u16vec2{ 3,4 }) = true;
    // std::vector<float> yaa{std::size_t(5000) * uint16_t(3), std::numeric_limits<float>::infinity() };
}

ubv::fvec2 rotate(ubv::fvec2 t_point, ubv::fvec2 t_origin, float t_angle)
{
    float s = std::sin(t_angle);
    float c = std::cos(t_angle);

    // translate point back to origin
    t_point.x -= t_origin.x;
    t_point.y -= t_origin.y;

    // rotate point
    float xnew = t_point.x * c - t_point.y * s;
    float ynew = t_point.x * s + t_point.y * c;

    // translate point back
    t_point.x = xnew + t_origin.x;
    t_point.y = ynew + t_origin.y;
    return t_point;
}

void draw_loop(ubv::Window *window, ubv::Texture &texture1, ubv::fmat4x4 &projection, std::vector<ubv::Model *> models,
               const ubv::Texture &grass_texture, const ubv::Texture &skybox1_texture, const ubv::Texture& skybox2_texture, const ubv::Texture& skybox3_texture, const ubv::Texture& skybox4_texture,
               const ubv::Texture &skybox_top_texture, const ubv::Texture &skybox_bottom_texture) noexcept
{
    const Timepoint t1;
    ubv::FrameBuffer frame_buffer(window->get_win_width(), window->get_win_height());
    frame_buffer.fog_params = ubv::FrameBuffer::FogParams{
        9.9F,                    // fog start
        10.0F,                   // fog end
        ubv::Pixel{255, 0, 0}, // fog color
        1.0F,                    // fog destiny
        true                     // fog enabled
    };
    frame_buffer.set_multisample(1);
    bool rotateMode = false;
    bool renderZBuffer = false;
    while (true)
    {
        frame_buffer.clear();
        static FpsCounter fps_counter;
        static Timepoint t3 = t1;
        const Timepoint t2;
        float elapsed_time = (t2 - t1);
        float delta_time = (t2 - t3) * 2.0F;
        std::cout << "FPS: " << fps_counter.update(t2) << '\n';
        t3 = t2;
        static ubv::fvec3 camera_position{0.0F, 0.0F, 0.0F};
        static ubv::fvec3 camera_pitch_yaw_roll{0.0F, ubv::degrees_to_radians(-90.0F), 0.0F};

        static unsigned frame_number = 0;

        // camera_pitch_yaw_roll.y = ubv::degrees_to_radians(elapsed_time * 60.0F * 1.0F);

        // static const auto camera_front = ubv::fvec3(0.0f, 0.0f, -1.0f);
        static const auto camera_up = ubv::fvec3(0.0f, 1.0f, 0.0f);

        /*static uint16_t stencil_value_u16 = 0;
        stencil_value_u16 += 10;
        stencil_value_u16 %= 512;
        unsigned wartosc = stencil_value_u16;
        if (stencil_value_u16 > 255)
        {
            wartosc = 256 - stencil_value_u16;
        }
        frame_buffer.stencil_value = wartosc;
        std::cout << unsigned(frame_buffer.stencil_value) << std::endl;*/

        static auto &model_rekin = *models.at(1);

        auto offset = rotate(ubv::fvec2(1.5F, 0.0F), ubv::fvec2(0.0F, 0.0F), elapsed_time);

        auto rekin_model_matrix = ubv::identity<float>();
        rekin_model_matrix.translate(ubv::fvec3(offset.x, -1.7F + 0.25F + std::sin(elapsed_time * 4.0F) / 4.0F, offset.y));
        rekin_model_matrix.rotate(ubv::degrees_to_radians(90.0F), ubv::fvec3(-1.0F, 0.0F, 0.0F));
        rekin_model_matrix.rotate(elapsed_time + ubv::degrees_to_radians(-90.0F), ubv::fvec3(0.0F, 0.0F, -1.0F));
        rekin_model_matrix.scale(ubv::fvec3(0.037F, 0.037F, 0.037F));
        model_rekin.model_matrix = rekin_model_matrix;

        if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Enter)))
        {
            rotateMode = !rotateMode;
        }

        /*static float scale_factor = 1.0F;
        static float translation_factor = 0.0F;
        static float rotation_angle = 0.0F;

        if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Space)))
        {
            for (auto& model : models)
            {
                rotation_angle += delta_time / 10.0F;
            }
        }

        if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Escape)))
        {
            for (auto& model : models)
            {
                scale_factor += delta_time / 2.0F;
            }
        }

        if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Enter)))
        {
            for (auto& model : models)
            {
                translation_factor += delta_time * 4.0F;
            }
        }

        for (auto& model : models)
        {
            auto matrix = ubv::identity<float>();
            matrix.scale(ubv::fvec3(scale_factor, scale_factor / 4.0F + 0.75F, scale_factor));
            matrix.rotate(rotation_angle, ubv::fvec3(0.1F, 0.2F, 0.3F));
            matrix.translate(ubv::fvec3{ 0.0F,translation_factor,0.0F });
            model->model_matrix = matrix;
        }
        */
        if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Left)))
        {
            if (rotateMode)
            {
                camera_pitch_yaw_roll.x -= delta_time / 3.0F;
            }
            else
            {
                camera_position.x -= delta_time;
            }
        }

        if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Right)))
        {
            if (rotateMode)
            {
                camera_pitch_yaw_roll.x += delta_time / 3.0F;
            }
            else
            {
                camera_position.x += delta_time;
            }
        }

        if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Up)))
        {
            if (rotateMode)
            {
                camera_pitch_yaw_roll.z += delta_time / 3.0F;
            }
            else
            {
                camera_position.z += delta_time;
            }
        }

        if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Down)))
        {
            if (rotateMode)
            {
                camera_pitch_yaw_roll.z -= delta_time / 3.0F;
            }
            else
            {
                camera_position.z -= delta_time;
            }
        }

        if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::W)))
        {
            if (rotateMode)
            {
                camera_pitch_yaw_roll.y += delta_time / 3.0F;
            }
            else
            {
                camera_position.y += delta_time;
            }
        }

        if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::S)))
        {
            if (rotateMode)
            {
                camera_pitch_yaw_roll.y -= delta_time / 3.0F;
            }
            else
            {
                camera_position.y -= delta_time;
            }
        }

        camera_pitch_yaw_roll.x =
            std::clamp(camera_pitch_yaw_roll.x, ubv::degrees_to_radians(-89.0F), ubv::degrees_to_radians(89.0F));

        ubv::fvec3 front;
        front.x = std::cos((camera_pitch_yaw_roll.y)) * std::cos((camera_pitch_yaw_roll.x));
        front.y = std::sin((camera_pitch_yaw_roll.x));
        front.z = std::sin((camera_pitch_yaw_roll.y)) * std::cos((camera_pitch_yaw_roll.x));

        const auto camera_front = ubv::normalize(front);

        auto view = ubv::look_at(camera_position, camera_position + camera_front, camera_up);
        // auto VP = view * projection;

        /*ubv::Vertex c0a{MVP * ubv::fvec4{ubv::fvec3(0, 0, 0), 1.0F}, ubv::fvec2{0, 0}};
        ubv::Vertex c0b{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 0), 1.0F }, ubv::fvec2{ 1, 0 } };
        ubv::Vertex c0c{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 0), 1.0F }, ubv::fvec2{ 0, 1 } };
        ubv::Vertex c0d{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 0), 1.0F }, ubv::fvec2{ 1, 1 } };
        ubv::Vertex c1a{ MVP * ubv::fvec4{ ubv::fvec3(0, 0, 1), 1.0F }, ubv::fvec2{ 0, 0 } };
        ubv::Vertex c1b{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 1), 1.0F }, ubv::fvec2{ 1, 0 } };
        ubv::Vertex c1c{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 1), 1.0F }, ubv::fvec2{ 0, 1 } };
        ubv::Vertex c1d{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 1), 1.0F }, ubv::fvec2{ 1, 1 } };
        ubv::Vertex c2a{ MVP * ubv::fvec4{ ubv::fvec3(0, 0, 0), 1.0F }, ubv::fvec2{ 0, 0 } };
        ubv::Vertex c2b{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 0), 1.0F }, ubv::fvec2{ 1, 0 } };
        ubv::Vertex c2c{ MVP * ubv::fvec4{ ubv::fvec3(0, 0, 1), 1.0F }, ubv::fvec2{ 0, 1 } };
        ubv::Vertex c2d{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 1), 1.0F }, ubv::fvec2{ 1, 1 } };
        ubv::Vertex c3a{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 0), 1.0F }, ubv::fvec2{ 0, 0 } };
        ubv::Vertex c3b{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 0), 1.0F }, ubv::fvec2{ 1, 0 } };
        ubv::Vertex c3c{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 1), 1.0F }, ubv::fvec2{ 0, 1 } };
        ubv::Vertex c3d{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 1), 1.0F }, ubv::fvec2{ 1, 1 } };
        ubv::Vertex c4a{ MVP * ubv::fvec4{ ubv::fvec3(0, 0, 0), 1.0F }, ubv::fvec2{ 0, 0 } };
        ubv::Vertex c4b{ MVP * ubv::fvec4{ ubv::fvec3(0, 0, 1), 1.0F }, ubv::fvec2{ 1, 0 } };
        ubv::Vertex c4c{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 0), 1.0F }, ubv::fvec2{ 0, 1 } };
        ubv::Vertex c4d{ MVP * ubv::fvec4{ ubv::fvec3(1, 0, 1), 1.0F }, ubv::fvec2{ 1, 1 } };
        ubv::Vertex c5a{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 0), 1.0F }, ubv::fvec2{ 0, 0 } };
        ubv::Vertex c5b{ MVP * ubv::fvec4{ ubv::fvec3(0, 1, 1), 1.0F }, ubv::fvec2{ 1, 0 } };
        ubv::Vertex c5c{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 0), 1.0F }, ubv::fvec2{ 0, 1 } };
        ubv::Vertex c5d{ MVP * ubv::fvec4{ ubv::fvec3(1, 1, 1), 1.0F }, ubv::fvec2{ 1, 1 } };

        const std::array<ubv::Vertex, 3> t0a{ c0b, c0c, c0a };
        const std::array<ubv::Vertex, 3> t0b{ c0b, c0c, c0d };

        const std::array<ubv::Vertex, 3> t1a{ c1b, c1c, c1a };
        const std::array<ubv::Vertex, 3> t1b{ c1b, c1c, c1d };

        const std::array<ubv::Vertex, 3> t2a{ c2b, c2c, c2a };
        const std::array<ubv::Vertex, 3> t2b{ c2b, c2c, c2d };

        const std::array<ubv::Vertex, 3> t3a{ c3b, c3c, c3a };
        const std::array<ubv::Vertex, 3> t3b{ c3b, c3c, c3d };

        const std::array<ubv::Vertex, 3> t4a{ c4b, c4c, c4a };
        const std::array<ubv::Vertex, 3> t4b{ c4b, c4c, c4d };

        const std::array<ubv::Vertex, 3> t5a{ c5b, c5c, c5a };
        const std::array<ubv::Vertex, 3> t5b{ c5b, c5c, c5d };

        std::vector<std::future<void>> tasks;

        frame_buffer.draw_triangle(t0a, texture1);
        frame_buffer.draw_triangle(t0b, texture1);
        frame_buffer.draw_triangle(t1a, texture1);
        frame_buffer.draw_triangle(t1b, texture1);
        frame_buffer.draw_triangle(t2a, texture1);
        frame_buffer.draw_triangle(t2b, texture1);
        frame_buffer.draw_triangle(t3a, texture1);
        frame_buffer.draw_triangle(t3b, texture1);
        frame_buffer.draw_triangle(t4a, texture1);
        frame_buffer.draw_triangle(t4b, texture1);
        frame_buffer.draw_triangle(t5a, texture1);
        frame_buffer.draw_triangle(t5b, texture1);*/

        // std::vector<std::array<ubv::Vertex, 3>> triangles_to_draw = model.m_triangles;
        // std::cout << triangles_to_draw.size() << std::endl;
        const auto pv_matrix = projection * view;
        std::vector<std::future<void>> tasks;

        frame_buffer.fog_params.enable = false;
        frame_buffer.depth_test = false;

        ubv::Vertex skyboxline1_a{ubv::fvec4(-1, -1, -1, 1.0F), ubv::fvec2(1.0F, 0.0F)};
        ubv::Vertex skyboxline1_b{ubv::fvec4(1, -1, -1, 1.0F), ubv::fvec2(0.0F, 0.0F)};
        ubv::Vertex skyboxline1_c{ubv::fvec4(1, 1, -1, 1.0F),  ubv::fvec2(0.0F, 1.0F)};
        ubv::Vertex skyboxline1_d{ubv::fvec4(-1, 1, -1, 1.0F), ubv::fvec2(1.0F, 1.0F)};

        ubv::Vertex skyboxline2_a{ubv::fvec4(-1, -1, 1, 1.0F), ubv::fvec2(0.0F, 0.0F)};
        ubv::Vertex skyboxline2_b{ubv::fvec4(1, -1, 1, 1.0F), ubv::fvec2(1.0F, 0.0F)};
        ubv::Vertex skyboxline2_c{ubv::fvec4(1, 1, 1, 1.0F), ubv::fvec2(1.0F, 1.0F)};
        ubv::Vertex skyboxline2_d{ubv::fvec4(-1, 1, 1, 1.0F), ubv::fvec2(0.0F, 1.0F)};

        ubv::Vertex skyboxline3_a{ubv::fvec4(-1, -1, -1, 1.0F), ubv::fvec2(0.0F, 0.0F)};
        ubv::Vertex skyboxline3_b{ubv::fvec4(-1, -1, 1, 1.0F), ubv::fvec2(1.0F, 0.0F)};
        ubv::Vertex skyboxline3_c{ubv::fvec4(-1, 1, 1, 1.0F), ubv::fvec2(1.0F, 1.0F)};
        ubv::Vertex skyboxline3_d{ubv::fvec4(-1, 1, -1, 1.0F), ubv::fvec2(0.0F, 1.0F)};

        ubv::Vertex skyboxline4_a{ubv::fvec4(1, -1, -1, 1.0F), ubv::fvec2(1.0F, 0.0F)};
        ubv::Vertex skyboxline4_b{ubv::fvec4(1, -1, 1, 1.0F), ubv::fvec2(0.0F, 0.0F)};
        ubv::Vertex skyboxline4_c{ubv::fvec4(1, 1, 1, 1.0F), ubv::fvec2(0.0F, 1.0F)};
        ubv::Vertex skyboxline4_d{ubv::fvec4(1, 1, -1, 1.0F), ubv::fvec2(1.0F, 1.0F)};

        ubv::Vertex skyboxlineT_a{ ubv::fvec4(-1, 1, -1, 1.0F), ubv::fvec2(0.0F, 0.0F) };
        ubv::Vertex skyboxlineT_b{ ubv::fvec4(1, 1, -1, 1.0F), ubv::fvec2(0.0F, 1.0F) };
        ubv::Vertex skyboxlineT_c{ ubv::fvec4(-1, 1, 1, 1.0F), ubv::fvec2(1.0F, 0.0F) };
        ubv::Vertex skyboxlineT_d{ ubv::fvec4(1, 1, 1, 1.0F), ubv::fvec2(1.0F, 1.0F) };

        ubv::Vertex skyboxlineB_a{ ubv::fvec4(-1, -1, -1, 1.0F), ubv::fvec2(0.0F, 1.0F) };
        ubv::Vertex skyboxlineB_b{ ubv::fvec4(1, -1, -1, 1.0F), ubv::fvec2(0.0F, 0.0F) };
        ubv::Vertex skyboxlineB_d{ ubv::fvec4(1, -1, 1, 1.0F), ubv::fvec2(1.0F, 0.0F) };
        ubv::Vertex skyboxlineB_c{ ubv::fvec4(-1, -1, 1, 1.0F), ubv::fvec2(1.0F, 1.0F) };

        ubv::Vertex *skybox_vertices[] = {&skyboxline1_a, &skyboxline1_b, &skyboxline1_c, &skyboxline1_d,
                                          &skyboxline2_a, &skyboxline2_b, &skyboxline2_c, &skyboxline2_d,
                                          &skyboxline3_a, &skyboxline3_b, &skyboxline3_c, &skyboxline3_d,
                                          &skyboxline4_a, &skyboxline4_b, &skyboxline4_c, &skyboxline4_d,
        & skyboxlineT_a,& skyboxlineT_b,& skyboxlineT_c,& skyboxlineT_d ,
        & skyboxlineB_a,& skyboxlineB_b,& skyboxlineB_c,& skyboxlineB_d };

        for (auto skybox_vertex : skybox_vertices)
        {
            skybox_vertex->position =
                pv_matrix * ubv::fvec4(ubv::fvec3(skybox_vertex->position) + camera_position, 1.0F);
        }

        std::array<ubv::Vertex, 3> skybox_triangle1_a{skyboxline1_a, skyboxline1_b, skyboxline1_c};
        std::array<ubv::Vertex, 3> skybox_triangle1_b{skyboxline1_a, skyboxline1_d, skyboxline1_c};

        std::array<ubv::Vertex, 3> skybox_triangle2_a{skyboxline2_a, skyboxline2_b, skyboxline2_c};
        std::array<ubv::Vertex, 3> skybox_triangle2_b{skyboxline2_a, skyboxline2_d, skyboxline2_c};

        std::array<ubv::Vertex, 3> skybox_triangle3_a{ skyboxline3_a, skyboxline3_b, skyboxline3_c };
        std::array<ubv::Vertex, 3> skybox_triangle3_b{ skyboxline3_a, skyboxline3_d, skyboxline3_c };

        std::array<ubv::Vertex, 3> skybox_triangle4_a{ skyboxline4_a, skyboxline4_b, skyboxline4_c };
        std::array<ubv::Vertex, 3> skybox_triangle4_b{ skyboxline4_a, skyboxline4_d, skyboxline4_c };

        std::array<ubv::Vertex, 3> skybox_triangleT_a{ skyboxlineT_a, skyboxlineT_d, skyboxlineT_b };
        std::array<ubv::Vertex, 3> skybox_triangleT_b{ skyboxlineT_a, skyboxlineT_d, skyboxlineT_c };

        std::array<ubv::Vertex, 3> skybox_triangleB_a{ skyboxlineB_a, skyboxlineB_d, skyboxlineB_b };
        std::array<ubv::Vertex, 3> skybox_triangleB_b{ skyboxlineB_a, skyboxlineB_d, skyboxlineB_c };

        tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
                                   skybox_triangle1_a, skybox3_texture, false));
        tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
                                   skybox_triangle1_b, skybox3_texture, false));

       tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
                                  skybox_triangle2_a, skybox1_texture, false));
        tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
                                  skybox_triangle2_b, skybox1_texture, false));

      tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
            skybox_triangle3_a, skybox4_texture, false));
      tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
          skybox_triangle3_b, skybox4_texture, false));

      tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
         skybox_triangle4_a, skybox2_texture, false));
      tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
          skybox_triangle4_b, skybox2_texture, false));

      tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
          skybox_triangleT_a, skybox_top_texture, false));
      tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
          skybox_triangleT_b, skybox_top_texture, false));

      tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
          skybox_triangleB_a, skybox_bottom_texture, false));
      tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
          skybox_triangleB_b, skybox_bottom_texture, false));

        for (const auto &task : tasks)
        {
            task.wait();
        }
        tasks.clear();

        frame_buffer.fog_params.enable = true;
        frame_buffer.depth_test = true;

        for (const auto &model : models)
        {
            for (const auto &[texture, triangles] : model->m_triangles)
            {
                auto triangles_to_draw = triangles;
                // std::cout << triangles_to_draw.size() << std::endl;
                for (auto &triangle : triangles_to_draw)
                {
                    for (auto &vertex : triangle)
                    {
                        vertex.position = pv_matrix * model->model_matrix * vertex.position;
                    }
                    // frame_buffer.draw_triangle(triangle, *model->m_textures[texture]);
                    tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
                                               triangle, *model->m_textures[texture], false));
                }
            }
        }

        constexpr float grass_y_offset = -3.16F;

        ubv::Vertex grass_a{ubv::fvec4(-50.0F, grass_y_offset, -50.0F, 1.0F), ubv::fvec2(0.0F, 0.0F)};
        ubv::Vertex grass_b{ubv::fvec4(50.0F, grass_y_offset, -50.0F, 1.0F), ubv::fvec2(8.0F, 0.0F)};
        ubv::Vertex grass_c{ubv::fvec4(50.0F, grass_y_offset, 50.0F, 1.0F), ubv::fvec2(8.0F, 8.0F)};
        ubv::Vertex grass_d{ubv::fvec4(-50.0F, grass_y_offset, 50.0F, 1.0F), ubv::fvec2(0.0F, 8.0F)};

        grass_a.position = pv_matrix * grass_a.position;
        grass_b.position = pv_matrix * grass_b.position;
        grass_c.position = pv_matrix * grass_c.position;
        grass_d.position = pv_matrix * grass_d.position;

        std::array<ubv::Vertex, 3> grass_triangle_a{grass_a, grass_b, grass_c};
        std::array<ubv::Vertex, 3> grass_triangle_b{grass_a, grass_d, grass_c};

        tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
                                   grass_triangle_a, grass_texture, false));
        tasks.push_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
                                   grass_triangle_b, grass_texture, false));

        // frame_buffer.draw_triangle(grass_triangle_a, grass_texture);
        // frame_buffer.draw_triangle(grass_triangle_b, grass_texture);

        for (const auto &task : tasks)
        {
            task.wait();
        }
        /*tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t0a,
        texture1)); tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
        t0b, texture1));

        tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t1a,
        texture1)); tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
        t1b, texture1));

        tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t2a,
        texture1)); tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
        t2b, texture1));

        tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t3a,
        texture1)); tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
        t3b, texture1));

        tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t4a,
        texture1)); tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
        t4b, texture1));

        tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, t5a,
        texture1)); tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer,
        t5b, texture1));*/

        /*const ubv::Vertex vertex_a{projection.multiply(ubv::fvec3(rotate(ubv::fvec2{0.5, -0.5}, ubv::fvec2{0.0, 0.0},
        0), 1.0F)), ubv::fvec2{-1, 0}}; // 1 0 const ubv::Vertex vertex_b{
        projection.multiply(ubv::fvec3(rotate(ubv::fvec2{  0.5,  0.5 }, ubv::fvec2{ 0.0, 0.0 }, 0), 1.0F)), ubv::fvec2{
        -1, -1 } }; const ubv::Vertex vertex_c{ projection.multiply(ubv::fvec3(rotate(ubv::fvec2{ -0.5,  0.5 },
        ubv::fvec2{ 0.0, 0.0 }, 0), 1.0F)), ubv::fvec2{ 0, -1 } }; const ubv::Vertex vertex_d{
        projection.multiply(ubv::fvec3(rotate(ubv::fvec2{ -0.5, -0.5 }, ubv::fvec2{ 0.0, 0.0 }, 0), 1.0F)), ubv::fvec2{
        0, 0 } };*/

        // const std::array<ubv::Vertex, 3> triangle1{ vertex_b, vertex_d, vertex_c };
        // const std::array<ubv::Vertex, 3> triangle2{	vertex_b, vertex_d, vertex_a };

        // tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle, &frame_buffer, triangle1,
        // texture1)); tasks.emplace_back(std::async(std::launch::async, &ubv::FrameBuffer::draw_triangle,
        // &frame_buffer, triangle2, texture1));

        // for(const auto& task : tasks) {
        //	task.wait();
        // }

        if (frame_number == 0)
        {
            // frame_buffer.draw_z_buffer();
            // frame_buffer.draw_to_stencil_buffer();
            // frame_buffer.stencil_test = true;
        }

        /*if (window->IsKeyPressed(ubv::keys.at(ubv::Keys::Space)))
            renderZBuffer = !renderZBuffer;

        if(renderZBuffer)
            frame_buffer.draw_z_buffer();*/

        frame_buffer.sample();
        window->display(frame_buffer);
        window->update();

       // frame_buffer.render_to_file("frames/bjutiful" + std::to_string(frame_number) + ".tga");
            ++frame_number;
        // frame_buffer.render_to_file("test.tga");
        /*frame_buffer.render_to_file([&]() {
            std::stringstream ss_frame_number;
            ss_frame_number << std::setfill('0') << std::setw(10) << frame_number;
            static std::string filename;
            filename = "frames/video_ms5_" + ss_frame_number.str() + ".tga";
            return filename;
        }());
        ++frame_number;
        if (frame_number == 420)
        {
            std::quick_exit(0);
        }*/
    }
}

namespace ubv
{
Sandbox::Sandbox(int t_argc, char **t_argv)
    : texture1{"res/grass1024.tga", Texture::FilteringType::LINEAR}, model_akwarium{"res/aqua.obj"},
      model_rekin{"res/Dorrie.obj"}, skybox1{"res/sunny1.tga", Texture::FilteringType::LINEAR},
    skybox2{ "res/sunny2.tga", Texture::FilteringType::LINEAR },
    skybox3{ "res/sunny3.tga", Texture::FilteringType::LINEAR },
    skybox4{ "res/sunny4.tga", Texture::FilteringType::LINEAR },
      skybox_top{"res/sunny_top.tga", Texture::FilteringType::LINEAR}, skybox_bottom{"res/sunny_bottom.tga",
                                                                                      Texture::FilteringType::LINEAR}
/*, texture2{ "res/test2.tga", Texture::FilteringType::NEAREST }*/ {
    skybox_bottom.clamp = true;
    skybox1.clamp = true;
    skybox2.clamp = true;
    skybox3.clamp = true;
    skybox4.clamp = true;
    skybox_top.clamp = true;
    for (auto i = 0; i < t_argc; ++i)
    {
        std::cout << "Program Input: " << t_argv[i] << "\n";
    }
}

Sandbox::~Sandbox()
{
}

void Sandbox::start()
{
    std::cout << "Hello UBVSR.\n";

#if defined(_WIN32)
    ubv::WindowWin32 window(ubv::WindowProps{640, 720, "Test UBVSR"});
#elif defined(__unix__)
    ubv::WindowX11 window(ubv::WindowProps{1280, 720, "Test UBVSR"});
#endif

    projection = fmat4x4(
        90.0F, static_cast<float>(window.get_win_width()) / static_cast<float>(window.get_win_height()), 0.1F, 200.0F);

    draw_loop(&window, texture1, projection, {&model_akwarium, &model_rekin}, texture1, skybox1, skybox2, skybox3, skybox4, skybox_top,
              skybox_bottom);

    std::cout << "Goodbye UBVSR\n";
    std::cin.get();
}
}; // namespace ubv

int main(int argc, char **argv)
try
{
    ubv::Sandbox app(argc, argv);
    app.start();
    return 0;
}
catch (const std::exception &exception)
{
    std::cout << "Fatal error!\n" << exception.what() << std::flush;
}
