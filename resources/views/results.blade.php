<!DOCTYPE html>
<html>
<head>
    <title>Search Results</title>
</head>
<body>

<h1>Search Results</h1>

@if (!empty($output) && isset($output->documents) && isset($output->timeRequired))
    <h2>Document Information</h2>
    <ul>
        @foreach($output->documents as $document)
            <li>
            <a href="{{ url('Rank-c/data/' . $document['filename']) }}" target="_blank">Filename: {{ $document['filename'] }}</a>, 
            </li>
        @endforeach
    </ul>

    <!-- <h2>Time Required</h2>
    <p>{{ $output->timeRequired }} milliseconds</p> -->
@else
    <p>No output available.</p>
@endif

</body>
</html>
