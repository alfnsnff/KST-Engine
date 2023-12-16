<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;

class SearchController extends Controller
{

    public function parseOutput($output)
    {
        $lines = explode("\n", $output);
    
        // Extract document information
        $documents = [];
        foreach ($lines as $line) {
            // Skip empty lines
            if (empty($line)) {
                continue;
            }
    
            $parts = explode("\t", $line);
    
            // Check if the array keys exist before accessing them
            $index = isset($parts[0]) ? intval($parts[0]) : 0;
            $filename = isset($parts[1]) ? trim($parts[1]) : '';
            $score = isset($parts[2]) ? floatval($parts[2]) : 0.0;
    
            // Only process lines with valid document information
            if ($index > 0 && $filename !== '') {
                $documents[] = [
                    'index' => $index,
                    'filename' => $filename,
                    'score' => $score,
                ];
            }
        }
    
        // Extract time information
        $timeLine = end($lines);
        $timeInfo = explode(' ', $timeLine);
    
        // Ensure there are enough elements before accessing the index
        $timeRequired = isset($timeInfo[count($timeInfo) - 2]) ? floatval($timeInfo[count($timeInfo) - 2]) : 0.0;
    
        // Create an object with the extracted information
        $resultObject = (object) [
            'documents' => $documents,
            'timeRequired' => $timeRequired,
        ];
    
        // For demonstration, you can return or use $resultObject as needed
        return $resultObject;
    }


    public function query(Request $request) {
        $input = $request->all();
        $parameter = $input['query'];
        $top_k = $input['top_k'];
        $currentWorkingDirectory = getcwd();
        if ($request->input('engine') === 'rank-c') {

            chdir(public_path('Rank-c'));
            $data = shell_exec(
                public_path('Rank-c/querydb.exe') . 
                ' ' . escapeshellarg($parameter) .
                ' ' . escapeshellarg($top_k + 1) . ' 2>&1'
            );
            $output = $this->parseOutput($data);

        } elseif ($request->input('engine') === 'swish-e') {


            dd(getcwd());
            $data = shell_exec(
                'swish-e -w' . 
                ' ' . escapeshellarg($parameter) . 
                ' ' . '-m' . 
                ' ' . escapeshellarg($top_k) . ' 2>&1'
            );
        }

        chdir($currentWorkingDirectory);
        return view('results')->with('output', $output); // Use the view() function here
    }
    
}
