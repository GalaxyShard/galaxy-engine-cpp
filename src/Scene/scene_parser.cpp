//#include <Galaxy/Scene/scene.hpp>
//#include <Galaxy/Renderer/texture.hpp>
//#include <Galaxy/object.hpp>
//#include <Galaxy/Renderer/shader.hpp>
//#include <Galaxy/Renderer/mesh.hpp>
//#include <Galaxy/Renderer/material.hpp>
//#include <Galaxy/Assets/assets.hpp>
//#include <iostream>
//#include <sstream>

//namespace {
//struct AssetVariable
//{
//    enum { MESH, TEX, SHADER, MAT, OBJ } type;
//    void *data;
//};
//struct ParseData
//{
//    Scene *scene;
//    int count=0;
//    std::unordered_map<std::string, std::string> arguments;
//    std::unordered_map<std::string, AssetVariable> variables;
//};
//void remove_first_last(std::string &str) { str = str.substr(1, str.size()-2); }
//void remove_quotes(std::string &str) { remove_first_last(str); }
//std::string parse_path(ParseData &data)
//{
//    assert(data.arguments.count("file"));
//    std::string &file = data.arguments["file"];
//    remove_quotes(file);

//    // expand directory, from GAME or GALAXY to the actual path
//    size_t firstSlash = file.find('/');
//    assert(firstSlash != std::string::npos);
//    std::string directory = file.substr(0, firstSlash);

//    if (directory == "GAME") { file.replace(0, 4, Assets::asset_path()); }
//    else if (directory == "GALAXY") { file.replace(0, 6, Assets::gasset_path()); }
//    else assert(false); // dont allow random file paths

//    return file;
//}
//// 1 parameter
//AssetVariable parse_mesh(ParseData &data)
//{
//    assert(data.arguments.count("file"));
//    Mesh *mesh = Mesh::from_obj(parse_path(data));
//    return {.data = mesh, .type = AssetVariable::MESH};
//}
//// 2 parameters
//AssetVariable parse_tex(ParseData &data)
//{
//    assert(data.arguments.count("file"));
//    assert(data.arguments.count("mode"));

//    auto mode = data.arguments["mode"] == "\"pixel\"" ? Texture::Pixel : Texture::Linear;
//    Texture *tex = new Texture(parse_path(data), mode);
//    return {.data = tex, .type = AssetVariable::TEX};
//}
//// 1 parameter
//AssetVariable parse_shader(ParseData &data)
//{
//    Shader *shader = new Shader(parse_path(data));
//    return {.data = shader, .type = AssetVariable::SHADER};
//}
//// 1~2 parameters
//AssetVariable parse_mat(ParseData &data)
//{
//    AssetVariable shaderv = data.variables[data.arguments["shader"]];
//    assert(shaderv.type == AssetVariable::SHADER);

//    Material *mat = new Material((Shader*)shaderv.data);

//    std::string &uniforms = data.arguments["uniforms"];
//    remove_first_last(uniforms);
//    auto ss_uniforms = std::stringstream(uniforms);

//    std::string name;
//    std::string value;
//    while(std::getline(ss_uniforms, name, ','))
//    {
//        {
//            // recreation of getline that skips tuples
//            char c;
//            bool isTuple = 0;
//            value.clear();
//            while (ss_uniforms.get(c) && (c != ',' || isTuple))
//            {
//                if (c == '(') isTuple = 1;
//                if (c == ')') isTuple = 0;
//                value.push_back(c);
//            }
//        }

//        remove_quotes(name);
//        if (value[0] == '(') // vector
//        {
//            remove_first_last(value);
//            auto ss_value = std::stringstream(value);
//            std::vector<float> vec;
            
//            std::string element;
//            while (std::getline(ss_value, element, ','))
//                vec.push_back(std::stof(element));
            
//            if (vec.size() == 2) mat->set_uniform(name.c_str(), Vector2(vec[0], vec[1]));
//            else if (vec.size() == 3) mat->set_uniform(name.c_str(), Vector3(vec[0], vec[1], vec[2]));
//            else if (vec.size() == 4) mat->set_uniform(name.c_str(), Vector4(vec[0], vec[1], vec[2], vec[3]));
//            else assert(false);
//        }
//        else if (value.find('.') != std::string::npos) // float
//            mat->set_uniform(name.c_str(), std::stof(value));
//        else // int
//            mat->set_uniform(name.c_str(), std::stoi(value));
        
//    }
//    return {.data = mat, .type = AssetVariable::MAT};
//}
//// 2~5 parameters
//AssetVariable parse_obj(ParseData &data)
//{
//    AssetVariable meshv = data.variables[data.arguments["mesh"]];
//    AssetVariable matv = data.variables[data.arguments["mat"]];
//    assert(meshv.type == AssetVariable::MESH);
//    assert(matv.type == AssetVariable::MAT);

//    Object *obj = new Object((Mesh*)meshv.data, (Material*)matv.data);

//    if (data.arguments.count("pos"))
//    {
//        std::string pos = data.arguments["pos"];

//        size_t x = pos.find(',');
//        assert(x != std::string::npos);

//        size_t y = pos.find(',', x+1);
//        assert(y != std::string::npos);

//        obj->position = Vector3(
//            std::stof(pos.substr(1, x-1)),
//            std::stof(pos.substr(x+1, y-x-1)),
//            std::stof(pos.substr(y+1, pos.size()-y-1)));

//    }
//    if (data.arguments.count("scale"))
//    {
//        std::string scale = data.arguments["scale"];
//        size_t x = scale.find(',');
//        assert(x != std::string::npos);

//        size_t y = scale.find(',', x+1);
//        assert(y != std::string::npos);

//        obj->scale = Vector3(
//            std::stof(scale.substr(1, x-1)),
//            std::stof(scale.substr(x+1, y-x-1)),
//            std::stof(scale.substr(y+1, scale.size()-y-1)));

//    }
//    //if (data.arguments.count("rotation"))
//    //{
//    //    std::string r_str = data.arguments["rotation"];
//    //    size_t x = r_str.find(',');
//    //    assert(x != std::string::npos);

//    //    size_t y = r_str.find(',', x+1);
//    //    assert(y != std::string::npos);

//    //    size_t z = r_str.find(',', y+1);
//    //    assert(z != std::string::npos);

//    //    obj->rotation = Quaternion(
//    //        std::stof(r_str.substr(1, x-1)),
//    //        std::stof(r_str.substr(x+1, y-x-1)),
//    //        std::stof(r_str.substr(y+1, z-y-1)),
//    //        std::stof(r_str.substr(y+1, r_str.size()-z-1)));
//    //}
//    return {.data = obj, .type = AssetVariable::OBJ};
//}

//}

//Scene* Scene::parse(const std::string &path, std::string name)
//{
//    /* SYNTAX
//        '@type' selects a variable type
//        all types: 'mesh' 'tex' 'shader' 'mat' 'obj'

//        'name{param=x, param2=y}'
//        creates a variable with the selected type & name,
//        parameters can be a variable by typing it's name or a string/number/tuple

//        Multiple types cannot be selected on the same line
//        Names cannot start or end with numbers or symbols
//    */
//    // TODO: implement a proper asset system
//    Scene *scene = new Scene(name);

//    AssetVariable (*parseFunc)(ParseData &);
//    ParseData parseData;
//    parseData.scene = scene;

//    auto stream = std::ifstream(path, std::ios::binary);

//    std::string line;
//    while(std::getline(stream, line))
//    {
//        if (line[0] == '#' || line.empty()) continue;

//        if (line[0] == '@')
//        {
//            std::string type = line.substr(1);

//            if (type == "mesh") parseFunc = &parse_mesh;
//            else if (type == "tex") parseFunc = &parse_tex;
//            else if (type == "shader") parseFunc = &parse_shader;
//            else if (type == "mat") parseFunc = &parse_mat;
//            else if (type == "obj") parseFunc = &parse_obj;
//            else assert(false);
//        }
//        else
//        {
//            // remove spaces
//            line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

//            size_t startBrace = line.find('{');
//            size_t endBrace = line.find('}');

//            assert(startBrace != std::string::npos);
//            assert(endBrace != std::string::npos);

//            std::string name = line.substr(0, startBrace);
            
//            parseData.arguments.clear();
//            parseData.count = 0;
            
//            std::vector<int> params, paramStarts, paramEnds;
//            paramStarts.push_back(startBrace);

//            for (int i = startBrace+1, inTuple=0; i < line.size(); ++i)
//            {
//                char c = line[i];
//                if (c == '(')
//                {
//                    ++inTuple;
//                    continue;
//                }
//                if (inTuple)
//                {
//                    if (c == ')') --inTuple;
//                    continue;
//                }
//                if (c == '=') params.push_back(i);
//                if (c == ',')
//                {
//                    paramStarts.push_back(i);
//                    paramEnds.push_back(i);
//                }
//            }
//            paramEnds.push_back(endBrace);
//            for (int i = 0; i < params.size(); ++i)
//            {
//                auto start = paramStarts[i] + 1;
//                auto middle = params[i];
//                auto end = paramEnds[i];

//                parseData.arguments[line.substr(start, middle-start)] = line.substr(middle+1, end-middle-1);
//            }
//            parseData.count = params.size();
//            assert(!parseData.variables.count(name));
//            parseData.variables[name] = parseFunc(parseData);
//        }
//    }
//    return scene;
//}