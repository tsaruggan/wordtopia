import React, { useState, useEffect } from 'react';

export default function Search({ getSuggestions, submitSearch }) {
  const [query, setQuery] = useState("");
  const [suggestions, setSuggestions] = useState([]);

  const handleSearchChange = (e) => {
    const lowercaseQuery = e.target.value.toLowerCase();
    setQuery(lowercaseQuery);
  };

  const fetchSuggestions = async (prefix) => {
    const newSuggestions = await getSuggestions(prefix);
    setSuggestions(newSuggestions);
  }

  const handleSubmit = (suggestion) => {
    setQuery("");
    submitSearch(suggestion);
  }

  useEffect(() => {
    const delay = 100;
    const debounceTimeout = setTimeout(() => {
      if (query.length > 0) {
        fetchSuggestions(query);
      } else {
        setSuggestions([]);
      }
    }, delay);
    return () => clearTimeout(debounceTimeout);
  }, [query]);

  return (
    <div className='searchContainer'>
      <input
        type="text"
        id="search"
        name="word"
        className='searchInput'
        placeholder="Search for words with similar vibes..."
        spellCheck={false}
        value={query}
        onChange={handleSearchChange}
        maxLength={50}
        autoComplete='off'
      />

      {suggestions.length > 0 && (
        <div className="suggestionsContainer">
          {suggestions.map((suggestion, index) => (
            <div 
              key={suggestion.word} 
              className="suggestionItem"
              onClick={() => handleSubmit(suggestion)}
            >
              <span id="suggestionTitle">{suggestion.word}</span>
              <span id="suggestionDefinition">{suggestion.definition}</span>
            </div>
          ))}
        </div>
      )}

    </div>
  );
}