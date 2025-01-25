import { useState } from 'react';
import Search from './Search';

function App() {
  const [queryResult, setQueryResult] = useState(null);
  const [searchResults, setSearchResults] = useState([]);

  const getSuggestions = async (prefix) => {
    const response = await fetch(`/suggest?prefix=${prefix}&n=8`);
    const data = await response.json();
    return data;
  }

  const getSearchResults = async (word) => {
    const response = await fetch(`/search?word=${word}&n=5`);
    const data = await response.json();
    return data;
  }

  const submitSearch = async (searchThing) => {
    setQueryResult(searchThing);
    const newSearchResults = await getSearchResults(searchThing.word);
    setSearchResults(newSearchResults);
    console.log(newSearchResults);
  }

  const renderContent = () => {
    if (!queryResult) {
      return (
        <Search
          getSuggestions={getSuggestions}
          submitSearch={submitSearch}
        />
      );
    } else {
      return (
        <>
          <div className="mainWordContainer">
            <span id="mainWordTitle">{queryResult.word}</span>
            <span id="mainWordDefinition">{queryResult.definition}</span>
          </div>
        </>
      );

    }
  }

  return (
    <>
      <div className='page'>
        <div className='main'>
          <div className='header'>
            <h1>Vibelang</h1>
          </div>
          {renderContent()}
        </div>
      </div>
    </>
  );
}

export default App;
