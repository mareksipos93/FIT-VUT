/*!
 * @file 
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include<math.h>
#include<assert.h>

#include"student/student_shader.h"
#include"student/gpu.h"
#include"student/uniforms.h"

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   ){
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3) ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret* definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní struktuře.<br>
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_getUniformsHandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()
    // extrakce dat
    const Vec3 *position = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
    const Vec3 *normal = vs_interpretInputVertexAttributeAsVec3(gpu, input, 1);
    const Uniforms uniforms = gpu_getUniformsHandle(gpu);

    // prevod do clip-space
    Mat4 mvp;
    multiply_Mat4_Mat4(&mvp, shader_interpretUniformAsMat4(uniforms, getUniformLocation(gpu, "projectionMatrix")), 
        shader_interpretUniformAsMat4(uniforms, getUniformLocation(gpu, "viewMatrix")));
    Vec4 pos4;
    copy_Vec3Float_To_Vec4(&pos4, position, 1.f);

    // zapis vysledku - clip-space
    multiply_Mat4_Vec4(&output->gl_Position, &mvp, &pos4);
    // zapis vysledku - pozice a normala
    Vec3 *out_position = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0);
    Vec3 *out_normal = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1);
    copy_Vec3(out_position, position);
    copy_Vec3(out_normal, normal);
    // zapis vysledku - gpu
    //output->gpu = gpu;
}

void phong_fragmentShader(
    GPUFragmentShaderOutput     *const output,
    GPUFragmentShaderInput const*const input ,
    GPU                          const gpu   ){
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br> 
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// Difuzní barvu materiálu nastavte na čistou zelenou.
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()

    // -c ../tests/output.bmp
    // -p
    // extrakce dat
    const Vec3 *position = fs_interpretInputAttributeAsVec3(gpu, input, 0);
    const Vec3 *normal = fs_interpretInputAttributeAsVec3(gpu, input, 1);
    Vec3 n_normal; normalize_Vec3(&n_normal, normal);
    const Uniforms uniforms = gpu_getUniformsHandle(gpu);
    const Vec3 *cameraPosition = shader_interpretUniformAsVec3(uniforms, getUniformLocation(gpu, "cameraPosition"));
    const Vec3 *lightPosition = shader_interpretUniformAsVec3(uniforms, getUniformLocation(gpu, "lightPosition"));

    // normalizovany vektor ke kamere - (V = norm(C - P))
    Vec3 toView;
    sub_Vec3(&toView, position, cameraPosition);
    normalize_Vec3(&toView, &toView);

    // normalizovany vektor ke zdroji svetla - (L = norm(L - P))
    Vec3 lightDir;
    sub_Vec3(&lightDir, lightPosition, position);
    normalize_Vec3(&lightDir, &lightDir);

    // odraz svetla podle zakona odrazu - (R = 2 (L . N) N - L)
    Vec3 r;
    reflect(&r, &lightDir, &n_normal);

    // pomocny vektor - (R . V)
    float r_dot_v = max(dot_Vec3(&r, &toView), 0.0f);

    // vypocet barvy podle phongovy intenzity
    output->color.data[0] = output->color.data[2] = (float)pow(r_dot_v, 40.0f);
    output->color.data[1] = output->color.data[0] + max(dot_Vec3(&lightDir, &n_normal), 0.0f);
    output->color.data[3] = 1.0f;
    //output->depth = input->depth;
}

/// @}
