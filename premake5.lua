--
-- Premake script (http://premake.github.io)
--

solution 'BenchmarkTimers'
    configurations  {'Debug', 'Release'}
    targetdir       'bin'
    
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
        
    project 'BenchmarkTimers'
        language    'C++'
        kind        'ConsoleApp'
        defines
        {
        }
        includedirs 
        { 
            'src',
        }

        files
        {
            'src/**.cpp',
            'src/**.h',
            'test/*.cpp',
        }
