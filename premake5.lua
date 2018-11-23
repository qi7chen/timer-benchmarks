--
-- Premake script (http://premake.github.io)
--

solution 'TimerqueueBenchmark'
    configurations  {'Debug', 'Release'}
    architecture 'x64'
    targetdir    'bin'
    
    filter 'configurations:Debug'
        defines { 'DEBUG' }
        symbols 'On'
        
    filter 'configurations:Release'
        defines { 'NDEBUG' }
        symbols 'On'
        optimize 'On'
                
    filter 'action:vs*'
        defines
        {
            'WIN32',
            'WIN32_LEAN_AND_MEAN',
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',
            'NOMINMAX',
        }
    filter "action:gmake"
        buildoptions '-std=c++11'
        links 'pthread'
        
    project 'libtimer'
        language    'C++'
        kind        'StaticLib'
        includedirs 
        { 
            'src',
        }
        files
        {
            'src/**.cpp',
            'src/**.h',
        }
        
        
    project 'benchmark'
        language    'C++'
        kind        'ConsoleApp'
        defines
        {
            'GTEST_STDLIB_CXX11',
            'GTEST_LANG_CXX11=1',
            'GTEST_HAS_TR1_TUPLE=0',
            'GTEST_HAS_STD_TUPLE_=1',
        }
        includedirs 
        { 
            'src',
            '3rd/googletest',
            '3rd/googletest/include',
        }
        files
        {
            'test/*.cpp',
            '3rd/googletest/src/gtest-all.cc',
        }
        links 'libtimer'

